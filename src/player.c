#include "../inc/game.h"

// Если зажаты вверх и вниз глючит

Player* allocPlayer() {
    return (Player*) MEM_alloc(sizeof(Player));
}

void player_init(Player* player, s16 startX, s16 startY, s16 mapOverheight) {
    // Спрайт
    player->sprite = SPR_addSprite(&player_sprite, startX, startY, TILE_ATTR(PLAYER_PALETTE, 0, FALSE, TRUE));
    PAL_setPalette(PLAYER_PALETTE, player_sprite.palette->data, DMA);
    // Положение и перемещение
    player->aabb.x.min = intToFastFix32(startX);
    player->aabb.y.min = intToFastFix32(startY + mapOverheight);
    player->aabb.x.max = player->aabb.x.min + FASTFIX32(PLAYER_WIDTH);
    player->aabb.y.max = player->aabb.y.min + FASTFIX32(PLAYER_WIDTH);
    player->posBuffer.x = player->aabb.x.min;
    player->posBuffer.y = player->aabb.y.min;
    engine_initAABBTileIndexes(&player->aabb);
    player->mapOverheight = mapOverheight;
    player->moving.x = DIRECTION_NONE;
    player->moving.y = DIRECTION_NONE;
    player->velocity.x = FASTFIX32(0);
    player->velocity.y = FASTFIX32(0);
    // Состояние
    player->isJumping = FALSE;
    player->isFalling = FALSE;
    player->onGround = FALSE;
}

void player_update(Player* player, u8* collisionsMap, u16 mapWTiles, u16 mapHTiles) {
    engine_drawInt("c", player->onCeiling, 30, 0);
    engine_drawInt("g", player->onGround, 30, 1);
    engine_drawInt("l", player->onLeftObstacle, 30, 2);
    engine_drawInt("r", player->onRightObstacle, 30, 3);
    engine_drawInt("vx", fastFix32ToInt(abs(player->velocity.x)), 30, 4);
    engine_drawInt("vy", fastFix32ToInt(abs(player->velocity.y)), 30, 5);
    engine_drawInt("mx", player->moving.x, 30, 6);
    engine_drawInt("my", player->moving.y, 30, 7);
    engine_drawInt("j", player->isJumping, 30, 8);
    engine_drawInt("f", player->isFalling, 30, 9);
    engine_drawInt("m", player->isMoving, 30, 10);

    player->isJumping = !player->onGround && player->velocity.y < FASTFIX32(0);
    player->isFalling = !player->onGround && player->velocity.y > FASTFIX32(0);
    player->isMoving  = player->velocity.x != FASTFIX32(0) || player->velocity.y != FASTFIX32(0);

    // Обновляем пространственную ориентацию спрайта
    if (player->moving.x == DIRECTION_RIGHT) {
        SPR_setHFlip(player->sprite, TRUE);
        player->facingDirection.x = 1;
    }
    if (player->moving.x == DIRECTION_LEFT) {
        SPR_setHFlip(player->sprite, FALSE);
        player->facingDirection.x = -1;
    }

    // Обновляем анимации на основе состояния персонажа
    if (player->isFalling) {
        player_setAnim(player, ANIM_STAND);
    } else if (player->isMoving) {
        player_setAnim(player, ANIM_WALK);
    } else {
        player_setAnim(player, ANIM_STAND);
    }

    // Добавляем гравитацию
    if (!player->onGround && player->moving.y == 0) {  //  
        //player->velocity.y = FASTFIX32(1);
    }

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
        return;
    }

    // На каждой итерации прибавляем скорость к "буферному" положению персонажа
    // Затем проверяем: если у буферного дробная часть равна нулю, то к нормальному положению прибавляем пиксель (velocity 1)

    // Нужно aabb игрока поменять на s16

    player->posBuffer.x += player->velocity.x;
    player->posBuffer.y += player->velocity.y;

    bool isMoveX = fastFix32Frac(player->posBuffer.x) == FASTFIX32(0);
    bool isMoveY = fastFix32Frac(player->posBuffer.y) == FASTFIX32(0);

    s16 pixelsX = 0;
    s16 pixelsY = 0;

    if (isMoveX) {
        if (abs(player->velocity.x) <= FASTFIX32(1)) {
            pixelsX = 1;
        } else if (abs(player->velocity.x) > FASTFIX32(1)) {
            pixelsX = fastFix32ToInt(abs(player->velocity.x));
        }
    }
    if (isMoveY) {
        if (abs(player->velocity.y) <= FASTFIX32(1)) {
            pixelsY = 1;
        } else if (abs(player->velocity.y) > FASTFIX32(1)) {
            pixelsY = fastFix32ToInt(abs(player->velocity.y));
        }
    }

    s8 dirX = 0;
    if (player->velocity.x > FASTFIX32(0)) {
        dirX = 1;
    } else if (player->velocity.x < FASTFIX32(0)) {
        dirX = -1;
    }
    s8 dirY = 0;
    if (player->velocity.y > FASTFIX32(0)) {
        dirY = 1;
    } else if (player->velocity.y < FASTFIX32(0)) {
        dirY = -1;
    }

    engine_drawInt("x", isMoveX, 30, 12);
    engine_drawInt("y", isMoveY, 30, 13);
    engine_drawInt("x", fastFix32ToInt(fastFix32Int(player->posBuffer.x)), 30, 14);
    engine_drawInt("y", fastFix32ToInt(fastFix32Int(player->posBuffer.y)), 30, 15);
    engine_drawInt("x", fastFix32ToInt(fastFix32Frac(player->posBuffer.x)), 30, 16);
    engine_drawInt("y", fastFix32ToInt(fastFix32Frac(player->posBuffer.y)), 30, 17);
    engine_drawInt("x", pixelsX, 30, 18);
    engine_drawInt("y", pixelsY, 30, 19);

    AABB nextPlayerAABB = player->aabb;

    s16 i;
    for (i = 0; i < pixelsY; i++) {
        // Находим AABB персонажа, для которого нужно проверить коллизии и принять решение о дальнейшем движении персонажа
        nextPlayerAABB = engine_shiftAABBy(nextPlayerAABB, dirY);
    }

    for (i = 0; i < pixelsX; i++) {
        // Находим AABB персонажа, для которого нужно проверить коллизии и принять решение о дальнейшем движении персонажаs
        nextPlayerAABB = engine_shiftAABBx(nextPlayerAABB, dirX);
    }


    // Надо написать функцию для расчета расстояния до ближайшего препятствия в пикселях
    // Текущий код будет работать только для скорости не больше 1

    engine_checkCollisions(nextPlayerAABB, collisionsMap, mapWTiles, mapHTiles, 
        &player->onLeftObstacle,
        &player->onRightObstacle,
        &player->onCeiling,
        &player->onGround);

    if (player->onLeftObstacle || player->onRightObstacle) {
        player->velocity.x = FASTFIX32(0);
    }
    if (player->onCeiling || player->onGround) {
        player->velocity.y = FASTFIX32(0);
    }

    // Присваиваем новое положение персонажа
    player->aabb = nextPlayerAABB;

    // Обновляем позицию спрайта на экране
    SPR_setPosition(player->sprite, fastFix32ToInt(player->aabb.x.min) - mapShiftX, fastFix32ToInt(player->aabb.y.min) - mapShiftY);

    if (isMoveX) {
        player->posBuffer.x = player->aabb.x.min;
    }
    if (isMoveY) {
        player->posBuffer.y = player->aabb.y.min;
    }
}

bool player_checkMapScrollX(Player* player, s16 mapShiftX, u16 mapW) {
    s16 playerX = SPR_getPositionX(player->sprite);
    return (
        player->isMoving &&                                                                           // Чтобы смещение происходило при движении персонажа
        player->onLeftObstacle == FALSE && player->onRightObstacle == FALSE &&                                     // Чтобы смещение останавливалось при столкновении с препятствием

        (player->moving.x > 0 || (mapShiftX - 1) >= 0) &&                                                  // Чтобы не смещать за левую границу
        (player->moving.x < 0 || mapShiftX + 1 <= (mapW - SCREEN_WIDTH)) &&                         // Чтобы не смещать за правую границу
        
        ((player->moving.x < 0 && playerX <= SCREEN_MIN_X) ||                                              // Чтобы смещать влево
        (player->moving.x > 0 && playerX + PLAYER_WIDTH >= SCREEN_MAX_X))                                  // Чтобы смещать вправо
    );
}

bool player_checkMapScrollY(Player* player, s16 mapShiftY, u16 mapH) {
    s16 playerY = SPR_getPositionY(player->sprite);
    return (
        player->isMoving &&                                                              // Чтобы смещение происходило при движении персонажа
        player->onCeiling == FALSE && player->onGround == FALSE &&                              // Чтобы смещение останавливалось при столкновении с препятствием

        (player->moving.y > 0 || (mapShiftY - 1) >= 0) &&                                     // Чтобы не смещать за верхнюю границу
        (player->moving.y < 0 || mapShiftY + 1 <= (mapH - SCREEN_HEIGHT)) &&           // Чтобы не смещать за нижнюю границу
        
        ((player->moving.y < 0 && playerY <= SCREEN_MIN_Y) ||                                 // Чтобы смещать вверх
        (player->moving.y > 0 && playerY + PLAYER_HEIGHT >= SCREEN_MAX_Y))                    // Чтобы смещать вниз
    );
}








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
    engine_drawInt("8", player->onCeiling == FALSE && player->onGround == FALSE, 30, 7);
    engine_drawInt("9", (player->direction.y == DIRECTION_DOWN || (mapShiftY - 1) >= 0), 30, 8);
    engine_drawInt("10", (player->direction.y == DIRECTION_UP || mapShiftY + 1 <= ((MAP_HEIGHT << 3) - SCREEN_HEIGHT)), 30, 9);
    engine_drawInt("11", (player->direction.y == DIRECTION_UP && playerY <= SCREEN_MIN_Y), 30, 10);
    engine_drawInt("12", (player->direction.y == DIRECTION_DOWN && playerY + PLAYER_HEIGHT >= SCREEN_MAX_Y), 30, 11);
    
    engine_drawInt("g", player->onGround, 30, 12);
    engine_drawInt("l", player->onLeftObstacle, 30, 13);
    engine_drawInt("r", player->onRightObstacle, 30, 14);
    engine_drawInt("c", player->onCeiling, 30, 15);
    */
/*
    engine_drawInt("1", player->aabb.x.min, 30, 0);
    engine_drawInt("2", player->aabb.y.min, 30, 1);
    engine_drawInt("3", SPR_getPositionX(player->sprite), 30, 2);
    engine_drawInt("4", SPR_getPositionY(player->sprite), 30, 3);
    engine_drawInt("5", mapShiftX, 30, 4);
    engine_drawInt("6", mapShiftY, 30, 5);
    engine_drawInt("7", player->direction.x != DIRECTION_NONE, 30, 6);
    engine_drawInt("8", player->onLeftObstacle == FALSE && player->onRightObstacle == FALSE, 30, 7);
    engine_drawInt("9", (player->direction.x == DIRECTION_RIGHT || (mapShiftX - 1) >= 0), 30, 8);
    engine_drawInt("10", (player->direction.x == DIRECTION_LEFT || mapShiftX + 1 <= ((MAP_WIDTH << 3) - 320)), 30, 9);
    engine_drawInt("11", (player->direction.x == DIRECTION_LEFT && playerX <= SCREEN_MIN_X), 30, 10);
    engine_drawInt("12", (player->direction.x == DIRECTION_RIGHT && playerX + PLAYER_WIDTH >= SCREEN_MAX_X), 30, 11);
    engine_drawInt("g", player->onGround, 30, 12);
    engine_drawInt("l", player->onLeftObstacle, 30, 13);
    engine_drawInt("r", player->onRightObstacle, 30, 14);
    engine_drawInt("c", player->onCeiling, 30, 15);
    */