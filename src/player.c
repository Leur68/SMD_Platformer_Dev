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
    player->posBuffer.x = intToFastFix32(player->aabb.x.min);
    player->posBuffer.y = intToFastFix32(player->aabb.y.min);
    engine_initAABBTileIndexes(&player->aabb);
    player->moving.x = DIRECTION_NONE;
    player->moving.y = DIRECTION_NONE;
    player->velocity.x = FASTFIX32(0);
    player->velocity.y = FASTFIX32(0);
    // Состояние
    player->isJumping = false;
    player->isFalling = false;
    player->inGround = false;
    // Сдвиг карты
    player->mapScrollFunc = mapScrollFunc;
}

void player_update(Player* player, u8* collisionsMap, u16 mapWTiles, u16 mapHTiles) {
    player->isJumping = !player->inGround && player->velocity.y < FASTFIX32(0);
    player->isFalling = !player->inGround && player->velocity.y > FASTFIX32(0);
    player->isMoving  = player->velocity.x != FASTFIX32(0) || player->velocity.y != FASTFIX32(0);

    // Обновляем пространственную ориентацию спрайта
    if (player->moving.x == DIRECTION_RIGHT) {
        SPR_setHFlip(player->sprite, true);
        player->facingDirection.x = 1;
    }
    if (player->moving.x == DIRECTION_LEFT) {
        SPR_setHFlip(player->sprite, false);
        player->facingDirection.x = -1;
    }

    // Обновляем анимации на основе состояния персонажа
    if (player->isFalling) {
        player_setAnim(player, ANIM_STAND);
    } else if (player->inGround && player->isMoving) {
        player_setAnim(player, ANIM_WALK);
    } else {
        player_setAnim(player, ANIM_STAND);
    }

    // Добавляем гравитацию
    #if (!DEBUG_COLLISIONS)
        if (!player->inGround) {
            if (player->velocity.y < FASTFIX32(GRAVITY)) {
                player->velocity.y += FASTFIX32(GRAVITY_ACCELERATION);
            }
            if (player->velocity.y > FASTFIX32(GRAVITY)) {
                player->velocity.y = FASTFIX32(GRAVITY);
            }
            player->moving.y = DIRECTION_DOWN;
        } else {
            player->moving.y = DIRECTION_NONE;
        }
    #endif

    // Обрабатываем коллизии и перемещаем персонажа
    player_move(player, collisionsMap, mapWTiles, mapHTiles);
}

// Обновление анимации персонажа
void player_setAnim(Player* player, u16 animIndex) {
    if (player->animIndex != animIndex) {
        player->animIndex = animIndex;
        SPR_setAnim(player->sprite, player->animIndex);
    }
}

void player_move(Player* player, u8* collisionsMap, u16 mapWTiles, u16 mapHTiles) {
    // Не проверяем коллизии если персонаж стоит на месте
    if (!player->isMoving) {
        //return;
    }

    // Перемещаем персонажа субпикселями
    // На каждой итерации прибавляем скорость к "буферному" положению персонажа
    // Затем проверяем: если у буферного целая часть изменилась, то к нормальному положению прибавляем количество пикселей на которое изменилось буферное положение

    player->posBuffer.x += player->velocity.x;
    player->posBuffer.y += player->velocity.y;

    s16 newPosX = fastFix32ToInt(fastFix32Int(player->posBuffer.x));
    s16 newPosY = fastFix32ToInt(fastFix32Int(player->posBuffer.y));

    s16 movedPixelsX = (newPosX >= player->aabb.x.min) ? (newPosX - player->aabb.x.min) : (player->aabb.x.min - newPosX);
    movedPixelsX = player->moving.x >= 0 ? movedPixelsX : -movedPixelsX;
    s16 movedPixelsY = (newPosY >= player->aabb.y.min) ? (newPosY - player->aabb.y.min) : (player->aabb.y.min - newPosY);
    movedPixelsY = player->moving.y >= 0 ? movedPixelsY : -movedPixelsY;

    if (movedPixelsX == 0 && movedPixelsY == 0) {
        return;
    }

    engine_shiftAABB(&player->aabb, movedPixelsX, movedPixelsY);

    // Проверяем коллизии

    engine_checkCollisions(player->aabb, collisionsMap, mapWTiles, mapHTiles, player->moving,
        &player->inLeftObstacle,
        &player->inRightObstacle,
        &player->inCeiling,
        &player->inGround);

    #if (!DEBUG_COLLISIONS)
        // Выталкиваем персонажа в нужную сторону если он застрял в препятствии (такое случается только при скорости больше 1)
        s16 shiftX = 0;
        s16 shiftY = 0;
        if (player->inLeftObstacle > 1) {
            shiftX = player->inLeftObstacle - 1;
        }
        if (player->inRightObstacle > 1) {
            shiftX = -(player->inRightObstacle - 1);
        }
        if (player->inCeiling > 1) {
            shiftY = player->inCeiling - 1;
        }
        if (player->inGround > 1) {
            shiftY = -(player->inGround - 1);
        }
        if (shiftX != 0 || shiftY != 0) {
            engine_shiftAABB(&player->aabb, shiftX, shiftY);

            engine_checkCollisions(player->aabb, collisionsMap, mapWTiles, mapHTiles, player->moving,
                &player->inLeftObstacle,
                &player->inRightObstacle,
                &player->inCeiling,
                &player->inGround);
            
            // Откат буфера на предыдущую позицию обязателен
            player->posBuffer.x += FASTFIX32(shiftX);
            player->posBuffer.y += FASTFIX32(shiftY);
        }

        // Останавливаем скорость по соответствующей оси при столкновении с препятствием
        u8 x = player->inLeftObstacle + player->inRightObstacle;
        u8 y = player->inCeiling + player->inGround;
        if (x > y) {
            player->velocity.x = FASTFIX32(0);
        }
        if (y > x) {
            player->velocity.y = FASTFIX32(0);
        }
    #endif

    // Скроллим карту

    bool isScrollX = player_checkMapScrollX(player, MAP_WIDTH);
    bool isScrollY = player_checkMapScrollY(player, MAP_HEIGHT);

    player->mapScrollFunc(
        (isScrollX ? movedPixelsX : 0), 
        (isScrollY ? movedPixelsY : 0));

    // Обновляем позицию спрайта на экране
        
    SPR_setPosition(player->sprite, player->aabb.x.min - mapShiftX, player->aabb.y.min - mapShiftY);

    player_debug(player, movedPixelsX, movedPixelsY);
}

bool player_checkMapScrollX(Player* player, u16 mapW) {
    s16 playerX = SPR_getPositionX(player->sprite);
    return (
        player->isMoving &&                                                              // Чтобы смещение происходило при движении персонажа
        player->inLeftObstacle == false && player->inRightObstacle == false &&           // Чтобы смещение останавливалось при столкновении с препятствием

        (player->moving.x > 0 || (mapShiftX - 1) >= 0) &&                                // Чтобы не смещать за левую границу
        (player->moving.x < 0 || mapShiftX + 1 <= (mapW - SCREEN_WIDTH)) &&              // Чтобы не смещать за правую границу
        
        ((player->moving.x < 0 && playerX <= SCREEN_MIN_X) ||                            // Чтобы смещать влево
        (player->moving.x > 0 && playerX + PLAYER_WIDTH >= SCREEN_MAX_X))                // Чтобы смещать вправо
    );
}

bool player_checkMapScrollY(Player* player, u16 mapH) {
    s16 playerY = SPR_getPositionY(player->sprite);
    return (
        player->isMoving &&                                                              // Чтобы смещение происходило при движении персонажа
        player->inCeiling == false && player->inGround == false &&                       // Чтобы смещение останавливалось при столкновении с препятствием

        (player->moving.y > 0 || (mapShiftY - 1) >= 0) &&                                // Чтобы не смещать за верхнюю границу
        (player->moving.y < 0 || mapShiftY + 1 <= (mapH - SCREEN_HEIGHT)) &&             // Чтобы не смещать за нижнюю границу
        
        ((player->moving.y < 0 && playerY <= SCREEN_MIN_Y) ||                            // Чтобы смещать вверх
        (player->moving.y > 0 && playerY + PLAYER_HEIGHT >= SCREEN_MAX_Y))               // Чтобы смещать вниз
    );
}

void player_debug(Player* player, s16 movedPixelsX, s16 movedPixelsY) {
    #if (DEBUG_PLAYER)
        s16 i = 0; 

        engine_drawInt("g", player->inGround, 30, i++);
        engine_drawInt("l", player->inLeftObstacle, 30, i++);
        engine_drawInt("r", player->inRightObstacle, 30, i++);
        engine_drawInt("c", player->inCeiling, 30, i++);
        engine_drawInt("j", player->isJumping, 30, i++);
        engine_drawInt("f", player->isFalling, 30, i++);
        engine_drawInt("m", player->isMoving, 30, i++);

        engine_drawInt("x1", player->aabb.x.min, 30, i++);
        engine_drawInt("x2", player->aabb.x.max, 30, i++);
        engine_drawInt("y1", player->aabb.y.min, 30, i++);
        engine_drawInt("y2", player->aabb.y.max, 30, i++);

        engine_drawInt("sX", SPR_getPositionX(player->sprite), 30, i++);
        engine_drawInt("sY", SPR_getPositionY(player->sprite), 30, i++);

        engine_drawInt("mx", player->moving.x, 30, i++);
        engine_drawInt("my", player->moving.y, 30, i++);

        engine_drawInt("vx", fastFix32ToInt(fastFix32Int(abs(player->velocity.x))), 30, i++);
        engine_drawInt("vy", fastFix32ToInt(fastFix32Int(abs(player->velocity.y))), 30, i++);

        engine_drawInt("msX", mapShiftX, 30, i++);
        engine_drawInt("msY", mapShiftY, 30, i++);

        engine_drawInt("mpX", movedPixelsX, 30, i++);
        engine_drawInt("mpY", movedPixelsY, 30, i++);
    #endif

    if (isDebug) {
        waitMs(100);
    }
}




/*

if (player->inLeftObstacle > 1 || player->inRightObstacle > 1 || player->inCeiling > 1 || player->inGround > 1) {
        // Если персонаж застрял (прошел внутрь препятствия)

        if ((player->inLeftObstacle > 1) || (player->inRightObstacle > 1) || (player->inCeiling > 1) || (player->inGround > 1)) {
            // Если застревание только одной стороной, просто отодвигаем в обратную сторону на нужное количество пикселей

            s8 x = (player->inLeftObstacle + player->inRightObstacle - 1) * player->moving.x;

            s8 y = (player->inCeiling + player->inGround - 1) * player->moving.y;

            //engine_shiftAABB(&player->aabb, -x, -y);

            //player->posBuffer.x -= FASTFIX32(x);
            //player->posBuffer.y -= FASTFIX32(y);
        } else {
            // Если застревание несколькими сторонами, возвращаем на предыдущую успешную позицию, так как невозможно определить сторону для обратного сдвига

            //engine_shiftAABB(&player->aabb, -movedPixelsX, -movedPixelsY);

            //player->posBuffer = lastPosBuffer;
        }

        if (player->moving.x != 0) {
            //player->velocity.x = FASTFIX32(0);
        }
        if (player->moving.y != 0) {
            //player->velocity.y = FASTFIX32(0);
        }

        engine_checkCollisions(player->aabb, collisionsMap, mapWTiles, mapHTiles, player->moving,
            &player->inLeftObstacle,
            &player->inRightObstacle,
            &player->inCeiling,
            &player->inGround);
    } else {
        // Если персонаж столкнулся с препятствием
        if (player->inLeftObstacle > 0 && player->velocity.x < FASTFIX32(0)) {
            //player->velocity.x = FASTFIX32(0);
        }
        if (player->inRightObstacle > 0 && player->velocity.x > FASTFIX32(0)) {
            //player->velocity.x = FASTFIX32(0);
        }
        if (player->inCeiling > 0 && player->velocity.y < FASTFIX32(0)) {
            //player->velocity.y = FASTFIX32(0);
        }
        if (player->inGround > 0 && player->velocity.y > FASTFIX32(0)) {
            //player->velocity.y = FASTFIX32(0);
        }
    }


        SPR_setPosition(player->sprite, player->aabb.x.min - mapShiftX, player->aabb.y.min - mapShiftY);

        engine_drawInt("c", player->inCeiling, 20, 0);
        engine_drawInt("g", player->inGround, 20, 1);
        engine_drawInt("l", player->inLeftObstacle, 20, 2);
        engine_drawInt("r", player->inRightObstacle, 20, 3);

        engine_drawInt("1", (player->inLeftObstacle > 1) + (player->inRightObstacle > 1) + (player->inCeiling > 1) + (player->inGround > 1), 20, 4);
        engine_drawInt("x", (player->inLeftObstacle + player->inRightObstacle - 1) * player->moving.x, 20, 5);
        engine_drawInt("y", (player->inCeiling + player->inGround - 1) * player->moving.y, 20, 6);
        waitMs(10000);


s16 i = 0;

    for (i = 0; i < abs(movedPixelsX); i++) {
        engine_shiftAABB(&player->aabb, player->moving.x, 0);

        engine_checkCollisions(player->aabb, collisionsMap, mapWTiles, mapHTiles, 
            &player->inLeftObstacle,
            &player->inRightObstacle,
            &player->inCeiling,
            &player->inGround);

        if (player->inLeftObstacle > 0 && player->velocity.x < FASTFIX32(0)) {
            player->velocity.x = FASTFIX32(0);
            movedPixelsX = i + 1;
            break;
        }
        if (player->inRightObstacle > 0 && player->velocity.x > FASTFIX32(0)) {
            player->velocity.x = FASTFIX32(0);
            movedPixelsX = i + 1;
            break;
        }
    }

    for (i = 0; i < abs(movedPixelsY); i++) {
        engine_shiftAABB(&player->aabb, 0, player->moving.y);

        engine_checkCollisions(player->aabb, collisionsMap, mapWTiles, mapHTiles, 
            &player->inLeftObstacle,
            &player->inRightObstacle,
            &player->inCeiling,
            &player->inGround);

        if (player->inCeiling > 0 && player->velocity.y < FASTFIX32(0)) {
            player->velocity.y = FASTFIX32(0);
            movedPixelsY = i + 1;
        }
        if (player->inGround > 0 && player->velocity.y > FASTFIX32(0)) {
            player->velocity.y = FASTFIX32(0);
            movedPixelsY = i + 1;
        }
    }

while ((currDirX != 0 || currDirY != 0) && (movedPixelsX != 0 || movedPixelsY != 0)) {
        engine_shiftAABB(&player->aabb, currDirX, currDirY);

        engine_checkCollisions(player->aabb, collisionsMap, mapWTiles, mapHTiles, 
            &player->inLeftObstacle,
            &player->inRightObstacle,
            &player->inCeiling,
            &player->inGround);

        if (i == abs(movedPixelsX) - 1 || player->velocity.x == FASTFIX32(0)) {
            currDirX = 0;
        }
        if (i == abs(movedPixelsY) - 1 || player->velocity.y == FASTFIX32(0)) {
            currDirY = 0;
        }

        //BMP_drawText("wait", 30, 12);
        //waitMs(500);
        //BMP_clearText(30, 13, 4);

        i++;
    }

    if (player->inCeiling > 0 && player->velocity.y < FASTFIX32(0)) {
        player->velocity.y = FASTFIX32(0);
    }
    if (player->inGround > 0 && player->velocity.y > FASTFIX32(0)) {
        player->velocity.y = FASTFIX32(0);
    }
    if (player->inLeftObstacle > 0 && player->velocity.x < FASTFIX32(0)) {
        player->velocity.x = FASTFIX32(0);
    }
    if (player->inRightObstacle > 0 && player->velocity.x > FASTFIX32(0)) {
        player->velocity.x = FASTFIX32(0);
    }


engine_shiftAABB(&player->aabb, movedPixelsX, movedPixelsY);

    engine_checkCollisions(player->aabb, collisionsMap, mapWTiles, mapHTiles, 
        &player->inLeftObstacle,
        &player->inRightObstacle,
        &player->inCeiling,
        &player->inGround);





    if (player->inLeftObstacle > 1 || player->inRightObstacle > 1 || player->inCeiling > 1 || player->inGround > 1) {
        if (player->velocity.x > FASTFIX32(1)) {
            player->velocity.x = FASTFIX32(1);
        }
        if (player->velocity.x < FASTFIX32(-1)) {
            player->velocity.x = FASTFIX32(-1);
        }
        if (player->velocity.y > FASTFIX32(1)) {
            player->velocity.y = FASTFIX32(1);
        }
        if (player->velocity.y < FASTFIX32(-1)) {
            player->velocity.y = FASTFIX32(-1);
        }
    }

    if (player->inLeftObstacle > 1 || player->inRightObstacle > 1 || player->inCeiling > 1 || player->inGround > 1) {
        engine_shiftAABB(&player->aabb, -movedPixelsX, -movedPixelsY);

        engine_checkCollisions(player->aabb, collisionsMap, mapWTiles, mapHTiles, 
            &player->inLeftObstacle,
            &player->inRightObstacle,
            &player->inCeiling,
            &player->inGround);
    }





    s8 correctX = 0;
    s8 correctY = 0;

    if (player->inCeiling > 1 && player->inGround == 0) {

        correctY = player->inCeiling - 1;
        player->inCeiling = 1;
        player->inLeftObstacle = 0;
        player->inRightObstacle = 0;
        player->velocity.y = FASTFIX32(0);

    } else if (player->inGround > 1 && player->inCeiling == 0) {

        correctY = -(player->inGround - 1);
        player->inGround = 1;
        player->inLeftObstacle = 0;
        player->inRightObstacle = 0;
        player->velocity.y = FASTFIX32(0);

    } else if (player->inLeftObstacle > 1 && player->inRightObstacle == 0) {

        correctX = player->inLeftObstacle - 1;
        player->inLeftObstacle = 1;
        player->inCeiling = 0;
        player->inGround = 0;
        player->velocity.x = FASTFIX32(0);

    } else if (player->inRightObstacle > 1 && player->inLeftObstacle == 0) {

        correctX = -(player->inRightObstacle - 1);
        player->inRightObstacle = 1;
        player->inCeiling = 0;
        player->inGround = 0;
        player->velocity.x = FASTFIX32(0);

    }
    
    if (correctX != 0 || correctY != 0) {
        engine_shiftAABB(&player->aabb, correctX, correctY);
    }


    s16 currDirX = player->moving.x;
    s16 currDirY = player->moving.y;
    s16 i = 0;

    while (currDirX != 0 || currDirY != 0) {
        engine_shiftAABB(&player->aabb, currDirX, currDirY);

        engine_checkCollisions(player->aabb, collisionsMap, mapWTiles, mapHTiles, 
            &player->inLeftObstacle,
            &player->inRightObstacle,
            &player->inCeiling,
            &player->inGround);

        if (player->inLeftObstacle || player->inRightObstacle) {
            player->velocity.x = FASTFIX32(0);
        }
        if (player->inCeiling || player->inGround) {
            player->velocity.y = FASTFIX32(0);
        }

        if (i == abs(player->movedPixels.x) - 1 || player->velocity.x == FASTFIX32(0)) {
            currDirX = 0;
        }
        if (i == abs(player->movedPixels.y) - 1 || player->velocity.y == FASTFIX32(0)) {
            currDirY = 0;
        }

        

        //BMP_drawText("wait", 30, 12);
        //waitMs(500);
        //BMP_clearText(30, 13, 4);


        i++;
    }


engine_shiftAABB(&player->aabb, player->movedPixels.x, player->movedPixels.y);

    engine_checkCollisions(player->aabb, collisionsMap, mapWTiles, mapHTiles, 
        &player->inLeftObstacle,
        &player->inRightObstacle,
        &player->inCeiling,
        &player->inGround);

    if (FASTFIX32(MAX_VELOCITY) > 1) {
        s16 currX = player->aabb.x.min;
        s16 currY = player->aabb.y.min;

        if (player->inCeiling && player->movedPixels.y < -1) {
            s16 nextTileY = engine_roundUpByEight(currY);
            s16 extra = nextTileY - currY;
            if (extra != 0) {
                player->aabb.y.min += extra;
                player->aabb.y.max += extra;
                engine_initAABBTileIndexes(&player->aabb);

                engine_checkCollisions(player->aabb, collisionsMap, mapWTiles, mapHTiles, 
                    &player->inLeftObstacle,
                    &player->inRightObstacle,
                    &player->inCeiling,
                    &player->inGround);

                player->movedPixels.y += extra;
            }
        }

        if (player->inGround && player->movedPixels.y > 1) {
            s16 nextTileY = engine_roundUpByEight(currY);
            s16 extra = 8 - (nextTileY - currY);

            if (extra != 0) {
                player->aabb.y.min -= extra;
                player->aabb.y.max -= extra;
                engine_initAABBTileIndexes(&player->aabb);

                engine_checkCollisions(player->aabb, collisionsMap, mapWTiles, mapHTiles, 
                    &player->inLeftObstacle,
                    &player->inRightObstacle,
                    &player->inCeiling,
                    &player->inGround);

                player->movedPixels.y -= extra;
            }
        }

        if (player->inLeftObstacle && player->movedPixels.x < -1) {
            s16 nextTileX = engine_roundUpByEight(currX);
            s16 extra = nextTileX - currX;

            if (extra != 0) {
                player->aabb.x.min += extra;
                player->aabb.x.max += extra;
                engine_initAABBTileIndexes(&player->aabb);

                engine_checkCollisions(player->aabb, collisionsMap, mapWTiles, mapHTiles, 
                    &player->inLeftObstacle,
                    &player->inRightObstacle,
                    &player->inCeiling,
                    &player->inGround);

                player->movedPixels.x += extra;
            }

        }
        if (player->inRightObstacle && player->movedPixels.x > 1) {
            s16 nextTileX = engine_roundUpByEight(currX);
            s16 extra = 8 - (nextTileX - currX);

            if (extra != 0) {
                player->aabb.x.min -= extra;
                player->aabb.x.max -= extra;
                engine_initAABBTileIndexes(&player->aabb);

                engine_checkCollisions(player->aabb, collisionsMap, mapWTiles, mapHTiles, 
                    &player->inLeftObstacle,
                    &player->inRightObstacle,
                    &player->inCeiling,
                    &player->inGround);

                player->movedPixels.x -= extra;
            }
        }
    }

    if (player->inLeftObstacle && player->velocity.x < FASTFIX32(0)) {
        player->velocity.x = FASTFIX32(0);
    }
    if (player->inRightObstacle && player->velocity.x > FASTFIX32(0)) {
        player->velocity.x = FASTFIX32(0);
    }
    if (player->inCeiling && player->velocity.y < FASTFIX32(0)) {
        player->velocity.y = FASTFIX32(0);
    }
    if (player->inGround && player->velocity.y > FASTFIX32(0)) {
        player->velocity.y = FASTFIX32(0);
    }


// Проверяем и откатываем застревания (застревание возможно только при скорости больше 1)
    if (FASTFIX32(MAX_VELOCITY) > 1) {
        s16 currX = player->aabb.x.min;
        s16 currY = player->aabb.y.min;

        if (player->inCeiling && player->movedPixels.y < -1) {
            s16 nextTileY = engine_roundUpByEight(currY);
            s16 extra = nextTileY - currY;
            if (extra != 0) {
                player->aabb.y.min += extra;
                player->aabb.y.max += extra;
                engine_initAABBTileIndexes(&player->aabb);

                engine_checkCollisions(player->aabb, collisionsMap, mapWTiles, mapHTiles, 
                    &player->inLeftObstacle,
                    &player->inRightObstacle,
                    &player->inCeiling,
                    &player->inGround);

                player->movedPixels.y += extra;
            }
        }

        if (player->inGround && player->movedPixels.y > 1) {
            s16 nextTileY = engine_roundUpByEight(currY);
            s16 extra = 8 - (nextTileY - currY);

            if (extra != 0) {
                player->aabb.y.min -= extra;
                player->aabb.y.max -= extra;
                engine_initAABBTileIndexes(&player->aabb);

                engine_checkCollisions(player->aabb, collisionsMap, mapWTiles, mapHTiles, 
                    &player->inLeftObstacle,
                    &player->inRightObstacle,
                    &player->inCeiling,
                    &player->inGround);

                player->movedPixels.y -= extra;
            }
        }

        if (player->inLeftObstacle && player->movedPixels.x < -1) {
            s16 nextTileX = engine_roundUpByEight(currX);
            s16 extra = nextTileX - currX;

            if (extra != 0) {
                player->aabb.x.min += extra;
                player->aabb.x.max += extra;
                engine_initAABBTileIndexes(&player->aabb);

                engine_checkCollisions(player->aabb, collisionsMap, mapWTiles, mapHTiles, 
                    &player->inLeftObstacle,
                    &player->inRightObstacle,
                    &player->inCeiling,
                    &player->inGround);

                player->movedPixels.x += extra;
            }

        }
        if (player->inRightObstacle && player->movedPixels.x > 1) {
            s16 nextTileX = engine_roundUpByEight(currX);
            s16 extra = 8 - (nextTileX - currX);

            if (extra != 0) {
                player->aabb.x.min -= extra;
                player->aabb.x.max -= extra;
                engine_initAABBTileIndexes(&player->aabb);

                engine_checkCollisions(player->aabb, collisionsMap, mapWTiles, mapHTiles, 
                    &player->inLeftObstacle,
                    &player->inRightObstacle,
                    &player->inCeiling,
                    &player->inGround);

                player->movedPixels.x -= extra;
            }
        }
    }

/*

    engine_shiftAABB(&player->aabb, player->movedPixels.x - dirX, player->movedPixels.y - dirY);

    if (player->inLeftObstacle || player->inRightObstacle) {
        player->velocity.x = FASTFIX32(0);
    }
    if (player->inCeiling || player->inGround) {
        player->velocity.y = FASTFIX32(0);
    }

    // Проверяем и откатываем застревания (застревание возможно только при скорости больше 1)
    if (FASTFIX32(MAX_VELOCITY) > 1) {
        s16 currX = player->aabb.x.min;
        s16 currY = player->aabb.y.min;

        if (player->inCeiling && player->movedPixels.y < -1) {
            s16 nextTileY = engine_roundUpByEight(currY);
            s16 extra = nextTileY - currY;
            if (extra != 0) {
                //player->posBuffer.y += intToFastFix32(extra);

                engine_drawInt("1", player->aabb.y.min, 30, 1);
                engine_drawInt("2", player->aabb.y.max, 30, 2);

                player->aabb.y.min += extra;
                player->aabb.y.max += extra;
                engine_initAABBTileIndexes(&player->aabb);

                engine_checkCollisions(player->aabb, collisionsMap, mapWTiles, mapHTiles, 
                    &player->inLeftObstacle,
                    &player->inRightObstacle,
                    &player->inCeiling,
                    &player->inGround);

                engine_drawInt("3", player->aabb.y.min, 30, 3);
                engine_drawInt("4", player->aabb.y.max, 30, 4);
                engine_drawInt("5", player->movedPixels.x, 30, 5);
                engine_drawInt("6", player->movedPixels.y, 30, 6);
                engine_drawInt("7", extra, 30, 7);

                player->movedPixels.y += extra;

                engine_drawInt("8", player->movedPixels.x, 30, 8);
                engine_drawInt("9", player->movedPixels.y, 30, 9);

                BMP_drawText("wait", 30, 7);
                waitMs(5000);
                BMP_clearText(30, 7, 4);
            }

            

        }
        if (player->inGround && player->movedPixels.y > 1) {
            s16 nextTileY = engine_roundUpByEight(currY);
            s16 extra = 8 - (nextTileY - currY);

            if (extra != 0) {
                engine_drawInt("1", player->aabb.y.min, 30, 1);
                engine_drawInt("2", player->aabb.y.max, 30, 2);

                //player->posBuffer.y -= intToFastFix32(extra);
                player->aabb.y.min -= extra;
                player->aabb.y.max -= extra;
                engine_initAABBTileIndexes(&player->aabb);

                player->movedPixels.y -= extra;

                engine_checkCollisions(player->aabb, collisionsMap, mapWTiles, mapHTiles, 
                    &player->inLeftObstacle,
                    &player->inRightObstacle,
                    &player->inCeiling,
                    &player->inGround);

                engine_drawInt("3", player->aabb.y.min, 30, 3);
                engine_drawInt("4", player->aabb.y.max, 30, 4);
                engine_drawInt("5", player->movedPixels.x, 30, 5);
                engine_drawInt("6", player->movedPixels.y, 30, 6);
                engine_drawInt("7", extra, 30, 7);

                BMP_drawText("wait", 30, 7);
                //waitMs(10000);
                BMP_clearText(30, 7, 4);
            }

            

        }

        if (player->inLeftObstacle && player->movedPixels.x < -1) {
            s16 nextTileX = engine_roundUpByEight(currX);
            s16 extra = nextTileX - currX;

            if (extra != 0) {
                //player->posBuffer.x += intToFastFix32(extra);
                player->aabb.x.min += extra;
                player->aabb.x.max += extra;
                engine_initAABBTileIndexes(&player->aabb);

                engine_checkCollisions(player->aabb, collisionsMap, mapWTiles, mapHTiles, 
                    &player->inLeftObstacle,
                    &player->inRightObstacle,
                    &player->inCeiling,
                    &player->inGround);

                player->movedPixels.x += extra;
            }

        }
        if (player->inRightObstacle && player->movedPixels.x > 1) {
            s16 nextTileX = engine_roundUpByEight(currX);
            s16 extra = 8 - (nextTileX - currX);

            if (extra != 0) {
                //player->posBuffer.x -= intToFastFix32(extra);
                player->aabb.x.min -= extra;
                player->aabb.x.max -= extra;
                engine_initAABBTileIndexes(&player->aabb);

                engine_checkCollisions(player->aabb, collisionsMap, mapWTiles, mapHTiles, 
                    &player->inLeftObstacle,
                    &player->inRightObstacle,
                    &player->inCeiling,
                    &player->inGround);

                player->movedPixels.x -= extra;
            }
        }
    }
    */





    /*
    engine_drawInt("1", pixelsX, 30, 12);
    engine_drawInt("2", pixelsY, 30, 13);
    engine_drawInt("3", fastFix32ToInt(abs(FASTFIX32(-1.3))) == 2, 30, 14);
    engine_drawInt("4", fastFix32ToInt(abs(FASTFIX32(-1.4))) == 2, 30, 15);
    engine_drawInt("4", fastFix32ToInt(abs(FASTFIX32(-1.5))) == 2, 30, 16);
    engine_drawInt("5", fastFix32ToInt(abs(FASTFIX32(-1.6))) == 2, 30, 17);
    engine_drawInt("6", fastFix32ToInt(abs(FASTFIX32(-1.7))) == 2, 30, 18);
    engine_drawInt("7", fastFix32ToInt(abs(FASTFIX32(-1.8))) == 2, 30, 19);
    engine_drawInt("8", fastFix32ToInt(abs(FASTFIX32(-1.9))) == 2, 30, 20);
    engine_drawInt("9", fastFix32ToInt(abs(FASTFIX32(-2.0))) == 2, 30, 21);
    engine_drawInt("10", fastFix32ToInt(abs(FASTFIX32(-2.1))) == 2, 30, 22);
    engine_drawInt("11", fastFix32ToInt(abs(FASTFIX32(-2.2))) == 2, 30, 23);
    engine_drawInt("12", fastFix32ToInt(abs(FASTFIX32(-2.3))) == 2, 30, 24);
    engine_drawInt("13", fastFix32ToInt(abs(FASTFIX32(-2.4))) == 2, 30, 25);
    */

        /**

        engine_drawInt("1", (nextPlayerAABB.xTiles.min), 30, 0);
        engine_drawInt("2", (nextPlayerAABB.xTiles.max), 30, 1);
        engine_drawInt("3", (nextPlayerAABB.yTiles.min), 30, 2);
        engine_drawInt("4", (nextPlayerAABB.yTiles.max), 30, 3);

        engine_drawInt("1", (aabbLeft.xTiles.min), 30, 4);
        engine_drawInt("2", (aabbLeft.xTiles.max), 30, 5);
        engine_drawInt("3", (aabbLeft.yTiles.min), 30, 6);
        engine_drawInt("4", (aabbLeft.yTiles.max), 30, 7);

        engine_drawInt("1", (aabbRight.xTiles.min), 30, 8);
        engine_drawInt("2", (aabbRight.xTiles.max), 30, 9);
        engine_drawInt("3", (aabbRight.yTiles.min), 30, 10);
        engine_drawInt("4", (aabbRight.yTiles.max), 30, 11);

        engine_drawInt("1", (aabbTop.xTiles.min), 30, 12);
        engine_drawInt("2", (aabbTop.xTiles.max), 30, 13);
        engine_drawInt("3", (aabbTop.yTiles.min), 30, 14);
        engine_drawInt("4", (aabbTop.yTiles.max), 30, 15);

        engine_drawInt("1", (aabbBottom.xTiles.min), 30, 16);
        engine_drawInt("2", (aabbBottom.xTiles.max), 30, 17);
        engine_drawInt("3", (aabbBottom.yTiles.min), 30, 18);
        engine_drawInt("4", (aabbBottom.yTiles.max), 30, 19);

        //waitMs(1000);
*/
    /*

    engine_drawInt("R1", aabbRight.xTiles.min, 30, 4);
    engine_drawInt("R2", aabbRight.xTiles.max, 30, 5);
    engine_drawInt("R3", aabbRight.yTiles.min, 30, 6);
    engine_drawInt("R4", aabbRight.yTiles.max, 30, 7);

    engine_drawInt("1", player->aabb.x.min, 30, 0);
    engine_drawInt("2", player->aabb.x.max, 30, 1);
    engine_drawInt("3", player->aabb.y.min, 30, 2);
    engine_drawInt("4", player->aabb.y.max, 30, 3);

    engine_drawInt("5", nextPlayerAABB.x.min, 30, 4);
    engine_drawInt("6", nextPlayerAABB.x.max, 30, 5);
    engine_drawInt("7", nextPlayerAABB.y.min, 30, 6);
    engine_drawInt("8", nextPlayerAABB.y.max, 30, 7);

    engine_drawInt("L1", aabbLeft.x.min, 30, 8);
    engine_drawInt("L2", aabbLeft.x.max, 30, 9);
    engine_drawInt("L3", aabbLeft.y.min, 30, 10);
    engine_drawInt("L4", aabbLeft.y.max, 30, 11);

    engine_drawInt("R1", aabbRight.x.min, 30, 12);
    engine_drawInt("R2", aabbRight.x.max, 30, 13);
    engine_drawInt("R3", aabbRight.y.min, 30, 14);
    engine_drawInt("R4", aabbRight.y.max, 30, 15);

    engine_drawInt("T1", aabbTop.x.min, 30, 16);
    engine_drawInt("T2", aabbTop.x.max, 30, 17);
    engine_drawInt("T3", aabbTop.y.min, 30, 18);
    engine_drawInt("T4", aabbTop.y.max, 30, 19);

    engine_drawInt("B1", aabbBottom.x.min, 30, 20);
    engine_drawInt("B2", aabbBottom.x.max, 30, 21);
    engine_drawInt("B3", aabbBottom.y.min, 30, 22);
    engine_drawInt("B4", aabbBottom.y.max, 30, 23);

    engine_drawInt("R1", aabbRight.xTiles.min, 30, 0);
    engine_drawInt("R2", aabbRight.xTiles.max, 30, 1);
    engine_drawInt("R3", aabbRight.yTiles.min, 30, 2);
    engine_drawInt("R4", aabbRight.yTiles.max, 30, 3);
    */
/*
    engine_drawInt("1", player->aabb.x.min, 30, 0);
    engine_drawInt("2", player->aabb.y.min, 30, 1);
    engine_drawInt("3", SPR_getPositionX(player->sprite), 30, 2);
    engine_drawInt("4", SPR_getPositionY(player->sprite), 30, 3);
    engine_drawInt("5", mapShiftX, 30, 4);
    engine_drawInt("6", mapShiftY, 30, 5);

    engine_drawInt("7", player->direction.y != DIRECTION_NONE, 30, 6);
    engine_drawInt("8", player->inCeiling == false && player->inGround == false, 30, 7);
    engine_drawInt("9", (player->direction.y == DIRECTION_DOWN || (mapShiftY - 1) >= 0), 30, 8);
    engine_drawInt("10", (player->direction.y == DIRECTION_UP || mapShiftY + 1 <= ((MAP_HEIGHT << 3) - SCREEN_HEIGHT)), 30, 9);
    engine_drawInt("11", (player->direction.y == DIRECTION_UP && playerY <= SCREEN_MIN_Y), 30, 10);
    engine_drawInt("12", (player->direction.y == DIRECTION_DOWN && playerY + PLAYER_HEIGHT >= SCREEN_MAX_Y), 30, 11);
    
    engine_drawInt("g", player->inGround, 30, 12);
    engine_drawInt("l", player->inLeftObstacle, 30, 13);
    engine_drawInt("r", player->inRightObstacle, 30, 14);
    engine_drawInt("c", player->inCeiling, 30, 15);
    */
/*
    engine_drawInt("1", player->aabb.x.min, 30, 0);
    engine_drawInt("2", player->aabb.y.min, 30, 1);
    engine_drawInt("3", SPR_getPositionX(player->sprite), 30, 2);
    engine_drawInt("4", SPR_getPositionY(player->sprite), 30, 3);
    engine_drawInt("5", mapShiftX, 30, 4);
    engine_drawInt("6", mapShiftY, 30, 5);
    engine_drawInt("7", player->direction.x != DIRECTION_NONE, 30, 6);
    engine_drawInt("8", player->inLeftObstacle == false && player->inRightObstacle == false, 30, 7);
    engine_drawInt("9", (player->direction.x == DIRECTION_RIGHT || (mapShiftX - 1) >= 0), 30, 8);
    engine_drawInt("10", (player->direction.x == DIRECTION_LEFT || mapShiftX + 1 <= ((MAP_WIDTH << 3) - 320)), 30, 9);
    engine_drawInt("11", (player->direction.x == DIRECTION_LEFT && playerX <= SCREEN_MIN_X), 30, 10);
    engine_drawInt("12", (player->direction.x == DIRECTION_RIGHT && playerX + PLAYER_WIDTH >= SCREEN_MAX_X), 30, 11);
    engine_drawInt("g", player->inGround, 30, 12);
    engine_drawInt("l", player->inLeftObstacle, 30, 13);
    engine_drawInt("r", player->inRightObstacle, 30, 14);
    engine_drawInt("c", player->inCeiling, 30, 15);
    */