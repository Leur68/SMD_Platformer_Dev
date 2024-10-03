#include "../inc/game.h"

Player* allocPlayer() {
    return (Player*) MEM_alloc(sizeof(Player));
}

void player_init(s16 startX, s16 startY) {
    // Спрайт
    player->sprite = SPR_addSpriteSafe(&player_sprite, startX, startY, TILE_ATTR(PLAYER_PALETTE, 0, false, true));
    PAL_setPalette(PLAYER_PALETTE, player_sprite.palette->data, DMA);
    // Положение и перемещение
    player->globalAABB.x.min = startX;
    player->globalAABB.y.min = startY + mapOverheight;
    player->globalAABB.x.max = player->globalAABB.x.min + PLAYER_WIDTH;
    player->globalAABB.y.max = player->globalAABB.y.min + PLAYER_WIDTH;
    engine_initAABBTileIndexes(&player->globalAABB);
    player->posBuffer.x = intToFastFix32(player->globalAABB.x.min);
    player->posBuffer.y = intToFastFix32(player->globalAABB.y.min);
    player->velocity.x = FASTFIX32(0);
    player->velocity.y = FASTFIX32(0);
    // Состояние
    player->isJumping = false;
    player->isFalling = false;
    player->inUpperObstacle = false;
    player->inLowerObstacle = false;
    player->inLeftObstacle = false;
    player->inRightObstacle = false;
    player->screenPos.x = startX;
    player->screenPos.y = startY;
}

void player_update() {
    player->isJumping = !player->inLowerObstacle && player->velocity.y < FASTFIX32(0);
    player->isFalling = !player->inLowerObstacle && player->velocity.y > FASTFIX32(0);
    player->isMoving  = player->velocity.x != FASTFIX32(0) || player->velocity.y != FASTFIX32(0);

    player->screenPos.x = SPR_getPositionX(player->sprite);
    player->screenPos.y = SPR_getPositionY(player->sprite);

    if (player->velocity.x > FASTFIX32(0)) {
        player->facingDirection.x = DIRECTION_RIGHT;
    } else if (player->velocity.x < FASTFIX32(0)) {
        player->facingDirection.x = DIRECTION_LEFT;
    }
    if (player->velocity.y > FASTFIX32(0)) {
        player->facingDirection.y = DIRECTION_DOWN;
    } else if (player->velocity.y < FASTFIX32(0)) {
        player->facingDirection.y = DIRECTION_UP;
    }

    // Обновляем пространственную ориентацию спрайта
    if (player->facingDirection.x == DIRECTION_RIGHT) {
        SPR_setHFlip(player->sprite, true);
    }
    if (player->facingDirection.x == DIRECTION_LEFT) {
        SPR_setHFlip(player->sprite, false);
    }

    // Обновляем анимации на основе состояния персонажа
    if (player->isFalling) {
        SPR_setAnim(player->sprite, ANIM_STAND);
    } else if (player->inLowerObstacle && player->isMoving) {
        SPR_setAnim(player->sprite, ANIM_WALK);
    } else {
        SPR_setAnim(player->sprite, ANIM_STAND);
    }

    // Добавляем гравитацию
    #if (!DEBUG_COLLISIONS)
        if (!player->inLowerObstacle) {
            if (player->velocity.y < FASTFIX32(GRAVITY)) {
                player->velocity.y += FASTFIX32(GRAVITY_ACCELERATION);
            }
            if (player->velocity.y > FASTFIX32(GRAVITY)) {
                player->velocity.y = FASTFIX32(GRAVITY);
            }
        }
    #endif

    u8 lastLower = player->inLowerObstacle;

    // Обрабатываем коллизии и перемещаем персонажа
    player_move();

    if (player->inLowerObstacle && player->coyoteTimer != 0) {
        player->coyoteTimer = 0; // Обнуляем таймер
    } else if (lastLower && !player->inLowerObstacle) {
        player->coyoteTimer = 1; // Запускаем таймер если персонаж сорвался вниз
    } else if (player->coyoteTimer > 0 && player->coyoteTimer <= MAX_COYOTE_TIME) { // Позволяем таймеру выйти за максимальный предел на единицу
        player->coyoteTimer++;
    }

    #if (DEBUG_PLAYER)
        player_debug();
    #endif
}

void player_move() {
    // Не проверяем коллизии если персонаж стоит на месте
    if (!player->isMoving) {
        return;
    }

    // Вычисляем кол-во пикселей для перемещения персонажа в текущем фрейме

    player_calculateVelocity();

    if (player->movedPixels.x == 0 && player->movedPixels.y == 0) {
        return;
    }

    // Меняем координаты персонажа

    engine_shiftAABB(&player->globalAABB, player->movedPixels);

    // Обрабатываем коллизии

    player_handleCollisions();

    // Скроллим карту

    camera_update();

    // Обновляем позицию спрайта на экране
        
    SPR_setPosition(player->sprite, player->globalAABB.x.min - cameraPosition.x, player->globalAABB.y.min - cameraPosition.y);
}

void player_calculateVelocity() {
    // Перемещаем персонажа субпикселями
    // На каждой итерации прибавляем скорость к "буферному" положению персонажа
    // Затем проверяем: если у буферного целая часть изменилась, то к нормальному положению прибавляем количество пикселей на которое изменилось буферное положение

    player->posBuffer.x += player->velocity.x;
    player->posBuffer.y += player->velocity.y;

    s16 newPosX = fastFix32ToInt(fastFix32Int(player->posBuffer.x));
    s16 newPosY = fastFix32ToInt(fastFix32Int(player->posBuffer.y));

    player->movedPixels.x = (newPosX >= player->globalAABB.x.min) ? (newPosX - player->globalAABB.x.min) : (player->globalAABB.x.min - newPosX);
    player->movedPixels.x = player->velocity.x >= 0 ? player->movedPixels.x : -player->movedPixels.x;
    player->movedPixels.y = (newPosY >= player->globalAABB.y.min) ? (newPosY - player->globalAABB.y.min) : (player->globalAABB.y.min - newPosY);
    player->movedPixels.y = player->velocity.y >= 0 ? player->movedPixels.y : -player->movedPixels.y;
}

void player_handleCollisions() {
    // Проверяем коллизии

    engine_checkCollisions(player->globalAABB, collisionsMap, mapWTiles, mapHTiles, player->facingDirection,
        &player->inLeftObstacle,
        &player->inRightObstacle,
        &player->inUpperObstacle,
        &player->inLowerObstacle);

    #if (!DEBUG_COLLISIONS)
        // Выталкиваем персонажа в нужную сторону если он застрял в препятствии (такое случается только при скорости больше 1)
        // Вычисляем величину выталкивания по двум осям

        Vect2D_s16 shift;
        shift.x = 0;
        shift.y = 0;
        if (player->inLeftObstacle > 1) {
            shift.x = player->inLeftObstacle - 1;
        }
        if (player->inRightObstacle > 1) {
            shift.x = -(player->inRightObstacle - 1);
        }
        if (player->inUpperObstacle > 1) {
            shift.y = player->inUpperObstacle - 1;
        }
        if (player->inLowerObstacle > 1) {
            shift.y = -(player->inLowerObstacle - 1);
        }

        if (shift.x != 0 || shift.y != 0) {
            // Выталкиваем
            engine_shiftAABB(&player->globalAABB, shift);

            // Заново рассчитываем коллизии
            engine_checkCollisions(player->globalAABB, collisionsMap, mapWTiles, mapHTiles, player->facingDirection,
                &player->inLeftObstacle,
                &player->inRightObstacle,
                &player->inUpperObstacle,
                &player->inLowerObstacle);

            // Корректируем переменные
            player->posBuffer.x += FASTFIX32(shift.x);
            player->posBuffer.y += FASTFIX32(shift.y);
            player->movedPixels.x += shift.x;
            player->movedPixels.y += shift.y;
        }

        // Останавливаем скорость по соответствующей оси при столкновении с препятствием
        if (player->inLeftObstacle || player->inRightObstacle) {
            player->velocity.x = FASTFIX32(0);
        }
        if (player->inUpperObstacle || player->inLowerObstacle) {
            player->velocity.y = FASTFIX32(0);
        }
    #endif
}

void player_debug() {
    s16 i = 0; 
    u16 x = 32;

    engine_drawDebugInt("u", player->inUpperObstacle, x, i++);
    engine_drawDebugInt("g", player->inLowerObstacle, x, i++);
    engine_drawDebugInt("l", player->inLeftObstacle, x, i++);
    engine_drawDebugInt("r", player->inRightObstacle, x, i++);
    engine_drawDebugInt("j", player->isJumping, x, i++);
    engine_drawDebugInt("f", player->isFalling, x, i++);
    engine_drawDebugInt("m", player->isMoving, x, i++);
    engine_drawDebugInt("ct", player->coyoteTimer, x, i++);
    engine_drawDebugInt("jt", player->jumpTimer, x, i++);

    engine_drawDebugInt("cx", cameraPosition.x, x, i++);
    engine_drawDebugInt("cy", cameraPosition.y, x, i++);
    engine_drawDebugUInt("bx", backPosition.x, x, i++);
    engine_drawDebugUInt("by", backPosition.y, x, i++);

    //engine_drawInt("gx1", player->globalAABB.x.min, 30, i++);
    //engine_drawInt("gx2", player->globalAABB.x.max, 30, i++);
    //engine_drawInt("gy1", player->globalAABB.y.min, 30, i++);
    //engine_drawInt("gy2", player->globalAABB.y.max, 30, i++);

    //engine_drawInt("sX", player->screenPos.x, 30, i++);
    //engine_drawInt("sY", player->screenPos.y, 30, i++);

    //engine_drawInt("dx", player->facingDirection.x, 30, i++);
    //engine_drawInt("dy", player->facingDirection.y, 30, i++);

    //engine_drawInt("vx", fastFix32ToInt(fastFix32Int(abs(player->velocity.x))), 30, i++);
    //engine_drawInt("vy", fastFix32ToInt(fastFix32Int(abs(player->velocity.y))), 30, i++);

    //engine_drawInt("cpX", cameraPosition.x, 30, i++);
    //engine_drawInt("cpY", cameraPosition.y, 30, i++);

    //engine_drawInt("mpX", player->movedPixels.x, 30, i++);
    //engine_drawInt("mpY", player->movedPixels.y, 30, i++);
}