#include "../inc/global.h"

Player* allocPlayer() {
    return (Player*) MEM_alloc(sizeof(Player));
}

void player_init(u16 startX, u16 startY) {
    // Спрайт
    player->sprite = SPR_addSpriteSafe(&player_sprite, startX, startY, TILE_ATTR(PLAYER_PALETTE, 0, false, true));
    SPR_setAlwaysOnTop(player->sprite);
    PAL_setPalette(PLAYER_PALETTE, player_sprite.palette->data, DMA);
    
    // Позиция
    player->globalAABB.x.min = startX;
    player->globalAABB.y.min = startY + mapMaxCameraPosY;
    player->globalAABB.x.max = player->globalAABB.x.min + PLAYER_WIDTH;
    player->globalAABB.y.max = player->globalAABB.y.min + PLAYER_WIDTH;
    aabb_initTileIndexes(&player->globalAABB);
    player->screenPos.x = startX;
    player->screenPos.y = startY;
    player->posBuffer.x = intToFastFix32(player->globalAABB.x.min);
    player->posBuffer.y = intToFastFix32(player->globalAABB.y.min);

    // Движение
    player->velocity.x = FASTFIX32(0);
    player->velocity.y = FASTFIX32(0);
    player->autoVelocity.x = FASTFIX32(0);
    player->autoVelocity.y = FASTFIX32(0);
    player->movedPixels.x = 0;
    player->movedPixels.y = 0;
    player->facingDirection = DIRECTION_NONE;
    player->coyoteTimer = 0;
    player->isMoving = false;
    player->isAutoMoving = false;
    player->isJumping = false;
    player->isFalling = false;

    // Коллизии
    player->inUpperObstacle = false;
    player->inLowerObstacle = false;
    player->inLeftObstacle = false;
    player->inRightObstacle = false;
}

// Инициализирует свойства игрока на текущем кадре
void player_update() {
    // Инициализируем начальное screenPos (до расчета коллизий) чтобы в купе с movedPixels это позволило camera_update() принять правильное решение о скроллинге
    // В player_update() не следует обновлять screenPos (после расчета коллизий и положения персонажа), так как это ответственность камеры
    player->screenPos.x = SPR_getPositionX(player->sprite);
    player->screenPos.y = SPR_getPositionY(player->sprite);

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

    // Обновляем направление
    player->facingDirection = DIRECTION_NONE;
    if (player->velocity.x < FASTFIX32(0)) {
        player->facingDirection |= DIRECTION_LEFT;

        SPR_setHFlip(player->sprite, false); // Обновляем пространственную ориентацию спрайта
    } else if (player->velocity.x > FASTFIX32(0)) {
        player->facingDirection |= DIRECTION_RIGHT;

        SPR_setHFlip(player->sprite, true); // Обновляем пространственную ориентацию спрайта
    }
    if (player->velocity.y < FASTFIX32(0)) {
        player->facingDirection |= DIRECTION_UP;
    }
    if (!player->isJumping) {
        player->facingDirection |= DIRECTION_DOWN;
    }

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

    // Эти значения следует рассчитывать именно здесь (после обработки коллизий)
    player->isMoving  = player->velocity.x != FASTFIX32(0) || player->velocity.y != FASTFIX32(0);
    player->isAutoMoving  = player->autoVelocity.x != FASTFIX32(0) || player->autoVelocity.y != FASTFIX32(0);
    player->isJumping = !player->inLowerObstacle && player->velocity.y < FASTFIX32(0);
    player->isFalling = !player->inLowerObstacle && player->velocity.y > FASTFIX32(0);

    // Обновляем анимации на основе состояния персонажа
    if (player->isFalling) {
        SPR_setAnim(player->sprite, ANIM_STAND);
    } else if (player->inLowerObstacle && player->isMoving) {
        SPR_setAnim(player->sprite, ANIM_WALK);
    } else {
        SPR_setAnim(player->sprite, ANIM_STAND);
    }
}

void player_move() {
    // Не проверяем коллизии если персонаж стоит на месте
    
    if (!player->isMoving && !player->isAutoMoving) {
        return;
    }

    // Вычисляем кол-во пикселей для перемещения персонажа в текущем фрейме

    player_calculateVelocity();

    if (player->movedPixels.x == 0 && player->movedPixels.y == 0) {
        return;
    }

    // Меняем координаты персонажа

    aabb_shift(&player->globalAABB, player->movedPixels);

    // Обрабатываем коллизии

    player_handleCollisions();

    #if (!DEBUG_COLLISIONS)
        // Останавливаем скорость по соответствующей оси при столкновении с препятствием
        if (player->inLeftObstacle || player->inRightObstacle) {
            player->velocity.x = FASTFIX32(0);
        }
        if (player->inUpperObstacle || player->inLowerObstacle) {
            player->velocity.y = FASTFIX32(0);
        }
    #endif
}

void player_calculateVelocity() {
    // Перемещаем персонажа субпикселями
    // На каждой итерации прибавляем скорость к "буферному" положению персонажа
    // Затем проверяем: если у буферного целая часть изменилась, то к нормальному положению прибавляем количество пикселей на которое изменилось буферное положение

    player->posBuffer.x += (player->velocity.x + player->autoVelocity.x);
    player->posBuffer.y += (player->velocity.y + player->autoVelocity.y);

    #if (DEBUG_INTERRUPT)
        if (player->posBuffer.x < FASTFIX32(0)) {
            SYS_die("player_calculateVelocity()", "posBuffer.x is negative", NULL);
        }
        if (player->posBuffer.y < FASTFIX32(0)) {
            SYS_die("player_calculateVelocity()", "posBuffer.y is negative", NULL);
        }
    #endif

    s16 newPosX = fastFix32ToInt(fastFix32Int(player->posBuffer.x));
    s16 newPosY = fastFix32ToInt(fastFix32Int(player->posBuffer.y));

    player->movedPixels.x = newPosX - player->globalAABB.x.min;
    player->movedPixels.y = newPosY - player->globalAABB.y.min;
}

void player_handleCollisions() {
    //if (player->facingDirection.y != DIRECTION_DOWN) {
    //    return;
    //}

    // Проверяем коллизии

    collision_check(player->globalAABB, player->facingDirection,
        &player->inLeftObstacle,
        &player->inRightObstacle,
        &player->inUpperObstacle,
        &player->inLowerObstacle);

    #if (!DEBUG_COLLISIONS)
        // Выталкиваем персонажа в нужную сторону если он застрял в препятствии (такое случается только при скорости больше 1)
        // Вычисляем величину выталкивания по двум осям

        Vect2D_s16 shift = {0, 0};
    
        if (player->inLeftObstacle > 1) {
            shift.x = player->inLeftObstacle - 1;
        } else if (player->inRightObstacle > 1) {
            shift.x = -(player->inRightObstacle - 1);
        }
        if (player->inUpperObstacle > 1) {
            shift.y = player->inUpperObstacle - 1;
        } else if (player->inLowerObstacle > 1) {
            shift.y = -(player->inLowerObstacle - 1);
        }

        if (shift.x != 0 || shift.y != 0) {
            // Выталкиваем
            aabb_shift(&player->globalAABB, shift);

            // Корректируем переменные
            player->posBuffer.x += FASTFIX32(shift.x);
            player->posBuffer.y += FASTFIX32(shift.y);
            player->movedPixels.x += shift.x;
            player->movedPixels.y += shift.y;

            // Заново рассчитываем коллизии
            collision_check(player->globalAABB, player->facingDirection,
                &player->inLeftObstacle,
                &player->inRightObstacle,
                &player->inUpperObstacle,
                &player->inLowerObstacle);
        }
    #endif
}