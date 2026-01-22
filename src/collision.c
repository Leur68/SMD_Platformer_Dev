#include "global.h"

Collider *allocCollider() {
    return (Collider *)MEM_alloc(sizeof(Collider));
}

void collider_init(Collider* collider, u16 x, u16 y) {
    aabb_set(&collider->globalAABB, x, y); // Position
    collider->groundCollisionData = 0;
    collider->tileCollisionFlags = 0;
}

u8 collision_getTileIndex(u16 xTile, u16 yTile) {
    if (xTile < 0 || yTile < 0 || xTile >= mapWTiles || yTile >= mapHTiles)
        return true;
    return mapPointerGet(collisionsMap, xTile, yTile);
}

bool collision_checkTileCollisions(AABB aabb, u16 *tileCollisionFlags) {

    for (u16 y = aabb.tileY.min; y <= aabb.tileY.max; y++) {
        for (u16 x = aabb.tileX.min; x <= aabb.tileX.max; x++) {

            u8 tileIndex = collision_getTileIndex(x, y);
            
            if (tileIndex != 0) {
                setBit(tileCollisionFlags, tileIndex); // Столкновение найдено
            }
        }
    }
}

bool collision_searchTileCollision(AABB aabb, u8 tileIndex) {

    for (u16 y = aabb.tileY.min; y <= aabb.tileY.max; y++) {
        for (u16 x = aabb.tileX.min; x <= aabb.tileX.max; x++) {

            u8 currTileIndex = collision_getTileIndex(x, y);
            
            if (currTileIndex == tileIndex) {
                return 1;
            }
        }
    }

    return 0;
}

s16 collision_moveX(Collider* collider, s16 intendedDeltaX) {

    AABB *globalAABB = &collider->globalAABB;
    u16 *groundCollisionData = &collider->groundCollisionData;

    clearBit(groundCollisionData, RIGHT_BIT_SHIFT);
    clearBit(groundCollisionData, LEFT_BIT_SHIFT);

    AABB predictedAABB = *globalAABB;
    aabb_shiftX(&predictedAABB, intendedDeltaX);

    if (!collision_searchTileCollision(predictedAABB, SOLID_TILE_INDEX)) {
        *globalAABB = predictedAABB;
        return intendedDeltaX;
    }
/*
    predictedAABB = *globalAABB;
    aabb_shiftX(&predictedAABB, (intendedDeltaX >= 0) ? 1 : -1);

    if (collision_searchTileCollision(predictedAABB, SOLID_TILE_INDEX)) {
        if (intendedDeltaX > 0) {
            setBit(groundCollisionData, RIGHT_BIT_SHIFT);
        } else if (intendedDeltaX < 0) {
            setBit(groundCollisionData, LEFT_BIT_SHIFT);
        }
        return 0;
    }
*/
    // остальные переменные
    s16 actualDeltaX = 0;
    s16 distanceX = abs(intendedDeltaX);
    s16 stepX = (intendedDeltaX >= 0) ? 1 : -1;
    AABB testAABB = *globalAABB;
    bool collidedWithSolid = false;
    s16 movedPixels = 0;

    for (s16 i = 1; i <= distanceX; i++) {

        aabb_shiftX(&testAABB, stepX);
        collidedWithSolid = collision_searchTileCollision(testAABB, SOLID_TILE_INDEX);

        if (collidedWithSolid) {
            actualDeltaX = (intendedDeltaX >= 0) ? i : -i;
            break;
        }
    }

    if (collidedWithSolid) {

        if (actualDeltaX > 0) {
            setBit(groundCollisionData, RIGHT_BIT_SHIFT);
        } else if (actualDeltaX < 0) {
            setBit(groundCollisionData, LEFT_BIT_SHIFT);
        }

        movedPixels = actualDeltaX - stepX; // Остановка на 1 пиксель раньше (работает для + и -)
    } else {
        movedPixels = intendedDeltaX;
    }

    if (movedPixels != 0) {
        aabb_shiftX(globalAABB, movedPixels);
    }

    return movedPixels;
}

s16 collision_moveY(Collider* collider, s16 intendedDeltaY) {

    AABB *globalAABB = &collider->globalAABB;
    u16 *groundCollisionData = &collider->groundCollisionData;

    clearBit(groundCollisionData, TOP_BIT_SHIFT);
    clearBit(groundCollisionData, BOTTOM_BIT_SHIFT);

    AABB predictedAABB = *globalAABB;
    aabb_shiftY(&predictedAABB, intendedDeltaY);

    if (!collision_searchTileCollision(predictedAABB, SOLID_TILE_INDEX)) {
        *globalAABB = predictedAABB;
        return intendedDeltaY;
    }
/*
    predictedAABB = *globalAABB;
    aabb_shiftY(&predictedAABB, (intendedDeltaY >= 0) ? 1 : -1);

    if (collision_searchTileCollision(predictedAABB, SOLID_TILE_INDEX)) {
        if (intendedDeltaY > 0) {
            setBit(groundCollisionData, BOTTOM_BIT_SHIFT);
        } else if (intendedDeltaY < 0) {
            setBit(groundCollisionData, TOP_BIT_SHIFT);
        }
        return 0;
    }
*/
    // остальные переменные
    s16 actualDeltaY = 0;
    s16 distanceY = abs(intendedDeltaY);
    s16 stepY = (intendedDeltaY >= 0) ? 1 : -1;
    AABB testAABB = *globalAABB;
    bool collidedWithSolid = false;
    s16 movedPixels = 0;

    for (s16 i = 1; i <= distanceY; i++) {

        aabb_shiftY(&testAABB, stepY);
        collidedWithSolid = collision_searchTileCollision(testAABB, SOLID_TILE_INDEX);

        if (collidedWithSolid) {
            actualDeltaY = (intendedDeltaY >= 0) ? i : -i;
            break;
        }
    }

    if (collidedWithSolid) {

        if (actualDeltaY > 0) {
            setBit(groundCollisionData, BOTTOM_BIT_SHIFT);
        } else if (actualDeltaY < 0) {
            setBit(groundCollisionData, TOP_BIT_SHIFT);
        }

        movedPixels = actualDeltaY - stepY; // Остановка на 1 пиксель раньше (работает для + и -)
    } else {
        movedPixels = intendedDeltaY;
    }

    if (movedPixels != 0) {
        aabb_shiftY(globalAABB, movedPixels);
    }

    return movedPixels;
}