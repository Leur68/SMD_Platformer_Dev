#include "../inc/game.h"

Player* allocPlayer() {
    return (Player*) MEM_alloc(sizeof(Player));
}

void player_init(Player* player, s16 startX, s16 startY, s16 mapOverheight) {
    // Спрайт
    player->sprite = SPR_addSprite(&player_sprite, startX, startY, TILE_ATTR(PLAYER_PALETTE, 0, FALSE, TRUE));
    PAL_setPalette(PLAYER_PALETTE, player_sprite.palette->data, DMA);
    // Положение и перемещение
    player->aabb.x.min = intToFix32(startX);
    player->aabb.y.min = intToFix32(startY + mapOverheight);
    player->aabb.x.max = player->aabb.x.min + FIX32(PLAYER_WIDTH);
    player->aabb.y.max = player->aabb.y.min + FIX32(PLAYER_WIDTH);
    engine_initAABBTileIndexes(&player->aabb);
    player->mapOverheight = mapOverheight;
    player->moving.x = DIRECTION_NONE;
    player->moving.y = DIRECTION_NONE;
    player->velocity.x = 0;
    player->velocity.y = 0;
    // Состояние
    player->isJumping = FALSE;
    player->isFalling = FALSE;
    player->onGround = FALSE;
}

void player_update(Player* player, u8* collisionsMap, u16 mapWTiles, u16 mapHTiles) {
    player->isJumping = !player->onGround && player->velocity.y < 0;
    player->isFalling = !player->onGround && player->velocity.y > 0;
    player->isMoving  = player->velocity.x != 0 || player->velocity.y != 0;

    // Добавляем гравитацию
    if (!player->onGround && player->moving.y == 0) {  //  
        player->velocity.y = 1;
    }

    // Обрабатываем коллизии и перемещаем персонажа
    player_move(player, collisionsMap, mapWTiles, mapHTiles);

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

    engine_drawInt("c", player->onCeiling, 30, 0);
    engine_drawInt("g", player->onGround, 30, 1);
    engine_drawInt("l", player->onLeftStuff, 30, 2);
    engine_drawInt("r", player->onRightStuff, 30, 3);
    engine_drawInt("vx", player->velocity.x, 30, 4);
    engine_drawInt("vy", player->velocity.y, 30, 5);
    engine_drawInt("dx", player->moving.x, 30, 6);
    engine_drawInt("dy", player->moving.y, 30, 7);

    engine_drawInt("j", player->isJumping, 30, 8);
    engine_drawInt("f", player->isFalling, 30, 9);
    engine_drawInt("m", player->isMoving, 30, 10);
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

    AABB nextPlayerAABB = player->aabb;

    s16 i = 0;
    Vect2D_s8 direction;
    direction.y = 0;
    if (player->velocity.y > 0) {
        direction.y = 1;
    }
    if (player->velocity.y < 0) {
        direction.y = -1;
    }
    direction.x = 0;

    for (i = 0; i < abs(player->velocity.y); i++) {
        // Находим AABB персонажа, для которого нужно проверить коллизии и принять решение о дальнейшем движении персонажа
        nextPlayerAABB = engine_getNewAABB(nextPlayerAABB, direction);

        // Находим ближайшие AABB с разных сторон от персонажа
        AABB aabbLeft   = engine_getLeftAABB(nextPlayerAABB);
        AABB aabbRight  = engine_getRightAABB(nextPlayerAABB);
        AABB aabbTop    = engine_getTopAABB(nextPlayerAABB);
        AABB aabbBottom = engine_getBottomAABB(nextPlayerAABB);

        // Отсеиваем среди найденных тайлов только те, что являются твердыми на карте для дальнейшей проверки столкновений
        aabbLeft   = engine_checkMapArea(collisionsMap, aabbLeft, mapWTiles, mapHTiles);
        aabbRight  = engine_checkMapArea(collisionsMap, aabbRight, mapWTiles, mapHTiles);
        aabbTop    = engine_checkMapArea(collisionsMap, aabbTop, mapWTiles, mapHTiles);
        aabbBottom = engine_checkMapArea(collisionsMap, aabbBottom, mapWTiles, mapHTiles);

        // Проверяем столкновения и останавливаем движение персонажа в случае если обнаружена коллизия
        player->onLeftStuff  = engine_isOverlappingAABBs(nextPlayerAABB, aabbLeft);
        player->onRightStuff = engine_isOverlappingAABBs(nextPlayerAABB, aabbRight);
        player->onCeiling    = engine_isOverlappingAABBs(nextPlayerAABB, aabbTop);
        player->onGround     = engine_isOverlappingAABBs(nextPlayerAABB, aabbBottom);

        if (player->onCeiling || player->onGround) {
            player->velocity.y = 0;
            break;
        }
    }

    direction.x = 0;
    if (player->velocity.x > 0) {
        direction.x = 1;
    }
    if (player->velocity.x < 0) {
        direction.x = -1;
    }
    direction.y = 0;
    for (i = 0; i < abs(player->velocity.x); i++) {
        // Находим AABB персонажа, для которого нужно проверить коллизии и принять решение о дальнейшем движении персонажа
        nextPlayerAABB = engine_getNewAABB(nextPlayerAABB, direction);

        // Находим ближайшие AABB с разных сторон от персонажа
        AABB aabbLeft   = engine_getLeftAABB(nextPlayerAABB);
        AABB aabbRight  = engine_getRightAABB(nextPlayerAABB);
        AABB aabbTop    = engine_getTopAABB(nextPlayerAABB);
        AABB aabbBottom = engine_getBottomAABB(nextPlayerAABB);

        // Отсеиваем среди найденных тайлов только те, что являются твердыми на карте для дальнейшей проверки столкновений
        aabbLeft   = engine_checkMapArea(collisionsMap, aabbLeft, mapWTiles, mapHTiles);
        aabbRight  = engine_checkMapArea(collisionsMap, aabbRight, mapWTiles, mapHTiles);
        aabbTop    = engine_checkMapArea(collisionsMap, aabbTop, mapWTiles, mapHTiles);
        aabbBottom = engine_checkMapArea(collisionsMap, aabbBottom, mapWTiles, mapHTiles);

        // Проверяем столкновения и останавливаем движение персонажа в случае если обнаружена коллизия
        player->onLeftStuff = engine_isOverlappingAABBs(nextPlayerAABB, aabbLeft);
        player->onRightStuff = engine_isOverlappingAABBs(nextPlayerAABB, aabbRight);
        player->onCeiling = engine_isOverlappingAABBs(nextPlayerAABB, aabbTop);
        player->onGround = engine_isOverlappingAABBs(nextPlayerAABB, aabbBottom);

        if (player->onLeftStuff || player->onRightStuff) {
            player->velocity.x = 0;
            break;
        }
    }

    // Присваиваем новое положение персонажа
    player->aabb = nextPlayerAABB;

    // Обновляем позицию спрайта на экране
    SPR_setPosition(player->sprite, fix32ToInt(player->aabb.x.min) - mapShiftX, fix32ToInt(player->aabb.y.min) - mapShiftY);
}

bool player_checkMapScrollX(Player* player, s16 mapShiftX, u16 mapW) {
    s16 playerX = SPR_getPositionX(player->sprite);
    return (
        player->velocity.x != 0 &&                                                                           // Чтобы смещение происходило при движении персонажа
        player->onLeftStuff == FALSE && player->onRightStuff == FALSE &&                                     // Чтобы смещение останавливалось при столкновении с препятствием

        (player->velocity.x > 0 || (mapShiftX - 1) >= 0) &&                                                  // Чтобы не смещать за левую границу
        (player->velocity.x < 0 || mapShiftX + 1 <= (mapW - SCREEN_WIDTH)) &&                         // Чтобы не смещать за правую границу
        
        ((player->velocity.x < 0 && playerX <= SCREEN_MIN_X) ||                                              // Чтобы смещать влево
        (player->velocity.x > 0 && playerX + PLAYER_WIDTH >= SCREEN_MAX_X))                                  // Чтобы смещать вправо
    );
}

bool player_checkMapScrollY(Player* player, s16 mapShiftY, u16 mapH) {
    s16 playerY = SPR_getPositionY(player->sprite);
    return (
        player->velocity.y != 0 &&                                                              // Чтобы смещение происходило при движении персонажа
        player->onCeiling == FALSE && player->onGround == FALSE &&                              // Чтобы смещение останавливалось при столкновении с препятствием

        (player->velocity.y > 0 || (mapShiftY - 1) >= 0) &&                                     // Чтобы не смещать за верхнюю границу
        (player->velocity.y < 0 || mapShiftY + 1 <= (mapH - SCREEN_HEIGHT)) &&           // Чтобы не смещать за нижнюю границу
        
        ((player->velocity.y < 0 && playerY <= SCREEN_MIN_Y) ||                                 // Чтобы смещать вверх
        (player->velocity.y > 0 && playerY + PLAYER_HEIGHT >= SCREEN_MAX_Y))                    // Чтобы смещать вниз
    );
}











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
    engine_drawInt("l", player->onLeftStuff, 30, 13);
    engine_drawInt("r", player->onRightStuff, 30, 14);
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
    engine_drawInt("8", player->onLeftStuff == FALSE && player->onRightStuff == FALSE, 30, 7);
    engine_drawInt("9", (player->direction.x == DIRECTION_RIGHT || (mapShiftX - 1) >= 0), 30, 8);
    engine_drawInt("10", (player->direction.x == DIRECTION_LEFT || mapShiftX + 1 <= ((MAP_WIDTH << 3) - 320)), 30, 9);
    engine_drawInt("11", (player->direction.x == DIRECTION_LEFT && playerX <= SCREEN_MIN_X), 30, 10);
    engine_drawInt("12", (player->direction.x == DIRECTION_RIGHT && playerX + PLAYER_WIDTH >= SCREEN_MAX_X), 30, 11);
    engine_drawInt("g", player->onGround, 30, 12);
    engine_drawInt("l", player->onLeftStuff, 30, 13);
    engine_drawInt("r", player->onRightStuff, 30, 14);
    engine_drawInt("c", player->onCeiling, 30, 15);
    */