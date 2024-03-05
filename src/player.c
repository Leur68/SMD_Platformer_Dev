#include "../inc/game.h"

Player* allocPlayer() {
    return (Player*) MEM_alloc(sizeof(Player));
}

void player_init(Player* player, s16 startX, s16 startY, s16 mapOverheight, MapScrollFunc* mapScrollFunc) {
    // Спрайт
    player->sprite = SPR_addSprite(&player_sprite, startX, startY, TILE_ATTR(PLAYER_PALETTE, 0, false, true));
    PAL_setPalette(PLAYER_PALETTE, player_sprite.palette->data, DMA);
    // Положение и перемещение
    player->aabb.x.min = startX;
    player->aabb.y.min = startY + mapOverheight;
    player->aabb.x.max = player->aabb.x.min + PLAYER_WIDTH;
    player->aabb.y.max = player->aabb.y.min + PLAYER_WIDTH;
    engine_initAABBTileIndexes(&player->aabb);
    player->posBuffer.x = intToFastFix32(player->aabb.x.min);
    player->posBuffer.y = intToFastFix32(player->aabb.y.min);
    player->velocity.x = FASTFIX32(0);
    player->velocity.y = FASTFIX32(0);
    // Состояние
    player->isJumping = false;
    player->isFalling = false;
    player->inUpperObstacle = false;
    player->inLowerObstacle = false;
    player->inLeftObstacle = false;
    player->inRightObstacle = false;
    // Сдвиг карты
    player->mapScrollFunc = mapScrollFunc;
}

void player_update(Player* player, u8* collisionsMap, u16 mapWTiles, u16 mapHTiles) {
    player->isJumping = !player->inLowerObstacle && player->velocity.y < FASTFIX32(0);
    player->isFalling = !player->inLowerObstacle && player->velocity.y > FASTFIX32(0);
    player->isMoving  = player->velocity.x != FASTFIX32(0) || player->velocity.y != FASTFIX32(0);

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

    // Обрабатываем коллизии и перемещаем персонажа
    player_move(player, collisionsMap, mapWTiles, mapHTiles);
}

void player_move(Player* player, u8* collisionsMap, u16 mapWTiles, u16 mapHTiles) {
    // Не проверяем коллизии если персонаж стоит на месте
    if (!player->isMoving) {
        return;
    }

    // Перемещаем персонажа субпикселями
    // На каждой итерации прибавляем скорость к "буферному" положению персонажа
    // Затем проверяем: если у буферного целая часть изменилась, то к нормальному положению прибавляем количество пикселей на которое изменилось буферное положение

    player->posBuffer.x += player->velocity.x;
    player->posBuffer.y += player->velocity.y;

    s16 newPosX = fastFix32ToInt(fastFix32Int(player->posBuffer.x));
    s16 newPosY = fastFix32ToInt(fastFix32Int(player->posBuffer.y));

    s16 movedPixelsX = (newPosX >= player->aabb.x.min) ? (newPosX - player->aabb.x.min) : (player->aabb.x.min - newPosX);
    movedPixelsX = player->velocity.x >= 0 ? movedPixelsX : -movedPixelsX;
    s16 movedPixelsY = (newPosY >= player->aabb.y.min) ? (newPosY - player->aabb.y.min) : (player->aabb.y.min - newPosY);
    movedPixelsY = player->velocity.y >= 0 ? movedPixelsY : -movedPixelsY;

    if (movedPixelsX == 0 && movedPixelsY == 0) {
        return;
    }

    // Меняем координаты персонажа

    engine_shiftAABB(&player->aabb, movedPixelsX, movedPixelsY);

    // Проверяем коллизии

    engine_checkCollisions(player->aabb, collisionsMap, mapWTiles, mapHTiles, player->facingDirection,
        &player->inLeftObstacle,
        &player->inRightObstacle,
        &player->inUpperObstacle,
        &player->inLowerObstacle);

    #if (!DEBUG_COLLISIONS)
        // Выталкиваем персонажа в нужную сторону если он застрял в препятствии (такое случается только при скорости больше 1)

        // Вычисляем величину выталкивания по двум осям
        s16 shiftX = 0;
        s16 shiftY = 0;
        if (player->inLeftObstacle > 1) {
            shiftX = player->inLeftObstacle - 1;
        }
        if (player->inRightObstacle > 1) {
            shiftX = -(player->inRightObstacle - 1);
        }
        if (player->inUpperObstacle > 1) {
            shiftY = player->inUpperObstacle - 1;
        }
        if (player->inLowerObstacle > 1) {
            shiftY = -(player->inLowerObstacle - 1);
        }

        if (shiftX != 0 || shiftY != 0) {
            // Выталкиваем
            engine_shiftAABB(&player->aabb, shiftX, shiftY);

            // Заново рассчитываем коллизии
            engine_checkCollisions(player->aabb, collisionsMap, mapWTiles, mapHTiles, player->facingDirection,
                &player->inLeftObstacle,
                &player->inRightObstacle,
                &player->inUpperObstacle,
                &player->inLowerObstacle);

            // Корректируем переменные
            player->posBuffer.x += FASTFIX32(shiftX);
            player->posBuffer.y += FASTFIX32(shiftY);
            movedPixelsX += shiftX;
            movedPixelsY += shiftY;
        }

        // Останавливаем скорость по соответствующей оси при столкновении с препятствием
        if (player->inLeftObstacle || player->inRightObstacle) {
            player->velocity.x = FASTFIX32(0);
        }
        if (player->inUpperObstacle || player->inLowerObstacle) {
            player->velocity.y = FASTFIX32(0);
        }
    #endif

    // Скроллим карту

    // Вызываем эти функции до вызова engine_checkCollisions чтобы скролл карты не остановился раньше времени в случае столкновения с препятствием
    bool isScrollX = player_isScrollMapX(player, MAP_WIDTH, movedPixelsX);
    bool isScrollY = player_isScrollMapY(player, MAP_HEIGHT, movedPixelsY);

    player->mapScrollFunc(
        (isScrollX ? movedPixelsX : 0), 
        (isScrollY ? movedPixelsY : 0));

    // Обновляем позицию спрайта на экране
        
    SPR_setPosition(player->sprite, player->aabb.x.min - mapShiftX, player->aabb.y.min - mapShiftY);

    #if (DEBUG_PLAYER)
        player_debug(player, movedPixelsX, movedPixelsY, isScrollX, isScrollY);
    #endif
}

bool player_isScrollMapX(Player* player, u16 mapW, s16 movedPixelsX) {
    s16 playerX = SPR_getPositionX(player->sprite);
    return (
        movedPixelsX != 0 &&                                                                                             // Скроллим карту только при движении персонажа

        (player->facingDirection.x == DIRECTION_RIGHT || (mapShiftX + movedPixelsX) >= 0) &&                             // Чтобы не смещать за левую границу
        (player->facingDirection.x == DIRECTION_LEFT  || (mapShiftX + movedPixelsX) <= (mapW - SCREEN_WIDTH)) &&         // Чтобы не смещать за правую границу
        
        ((player->facingDirection.x == DIRECTION_LEFT && playerX <= SCREEN_MIN_X) ||                                     // Чтобы смещать влево
        (player->facingDirection.x == DIRECTION_RIGHT && (playerX + PLAYER_WIDTH) >= SCREEN_MAX_X))                      // Чтобы смещать вправо
    );
}

bool player_isScrollMapY(Player* player, u16 mapH, s16 movedPixelsY) {
    s16 playerY = SPR_getPositionY(player->sprite);
    return (
        movedPixelsY != 0 &&                                                                                              // Скроллим карту только при движении персонажа

        (player->facingDirection.y == DIRECTION_DOWN || (mapShiftY + movedPixelsY) >= 0) &&                               // Чтобы не смещать за верхнюю границу
        (player->facingDirection.y == DIRECTION_UP   || (mapShiftY + movedPixelsY) <= (mapH - SCREEN_HEIGHT)) &&          // Чтобы не смещать за нижнюю границу
        
        ((player->facingDirection.y == DIRECTION_UP && playerY <= SCREEN_MIN_Y) ||                                        // Чтобы смещать вверх
        (player->facingDirection.y == DIRECTION_DOWN && playerY + PLAYER_HEIGHT >= SCREEN_MAX_Y))                         // Чтобы смещать вниз
    );
}

void player_debug(Player* player, s16 movedPixelsX, s16 movedPixelsY, bool isScrollX, bool isScrollY) {
    s16 i = 0; 

    engine_drawInt("u", player->inUpperObstacle, 30, i++);
    engine_drawInt("g", player->inLowerObstacle, 30, i++);
    engine_drawInt("l", player->inLeftObstacle, 30, i++);
    engine_drawInt("r", player->inRightObstacle, 30, i++);
    engine_drawInt("j", player->isJumping, 30, i++);
    engine_drawInt("f", player->isFalling, 30, i++);
    engine_drawInt("m", player->isMoving, 30, i++);

    engine_drawInt("x1", player->aabb.x.min, 30, i++);
    engine_drawInt("x2", player->aabb.x.max, 30, i++);
    engine_drawInt("y1", player->aabb.y.min, 30, i++);
    engine_drawInt("y2", player->aabb.y.max, 30, i++);

    engine_drawInt("sX", SPR_getPositionX(player->sprite), 30, i++);
    engine_drawInt("sY", SPR_getPositionY(player->sprite), 30, i++);

    engine_drawInt("dx", player->facingDirection.x, 30, i++);
    engine_drawInt("dy", player->facingDirection.y, 30, i++);

    engine_drawInt("vx", fastFix32ToInt(fastFix32Int(abs(player->velocity.x))), 30, i++);
    engine_drawInt("vy", fastFix32ToInt(fastFix32Int(abs(player->velocity.y))), 30, i++);

    engine_drawInt("msX", mapShiftX, 30, i++);
    engine_drawInt("msY", mapShiftY, 30, i++);

    //engine_drawInt("mpX", movedPixelsX, 30, i++);
    //engine_drawInt("mpY", movedPixelsY, 30, i++);

    //engine_drawInt("sx", isScrollX, 30, i++);
    //engine_drawInt("sy", isScrollY, 30, i++);
}