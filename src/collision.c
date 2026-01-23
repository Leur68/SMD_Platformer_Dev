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

s16 collision_computeDeltaX(AABB* aabb, s16 intendedDeltaX) {

    s16 stepX = (intendedDeltaX >= 0) ? 1 : -1;

    // если коллизии нет на желаемом расстоянии, то на следующем пикселе тоже нет
    // поэтому нет смысла выполнять и то и другое, просто возвращаем intendedDeltaX
    // если коллизия есть на желаемом расстоянии, то есть смысл проверить не находится ли она на следующем пикселе
    // если на следующем пикселе, достаточно просто вернуть 0
    
    // проверяем есть ли коллизия в желаемой точке

    AABB testAABB;
    testAABB = *aabb;
    aabb_shiftX(&testAABB, intendedDeltaX);

    if (!collision_searchTileCollision(testAABB, SOLID_TILE_INDEX)) {
        
        // если нет, выходим из функции

        return intendedDeltaX;

    } else {

        // если есть, проверяем не находится ли она на следующем пикселе

        testAABB = *aabb;
        aabb_shiftX(&testAABB, stepX);

        // если да, достаточно просто вернуть 0

        if (collision_searchTileCollision(testAABB, SOLID_TILE_INDEX)) {
            return 0;
        }

    }
    
    // проверяем есть ли коллизия пошагово

    // остальные переменные
    testAABB = *aabb;
    s16 actualDeltaX = 0;
    s16 distanceX = abs(intendedDeltaX);
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
        movedPixels = actualDeltaX - stepX; // Остановка на 1 пиксель раньше (работает для + и -)
    } else {
        movedPixels = intendedDeltaX;
    }

    return movedPixels;
}

s16 collision_computeDeltaY(AABB* aabb, s16 intendedDeltaY) {

    s16 stepY = (intendedDeltaY >= 0) ? 1 : -1;

    // если коллизии нет на желаемом расстоянии, то на следующем пикселе тоже нет
    // поэтому нет смысла выполнять и то и другое, просто возвращаем intendedDeltaX
    // если коллизия есть на желаемом расстоянии, то есть смысл проверить не находится ли она на следующем пикселе
    // если на следующем пикселе, достаточно просто вернуть 0
    
    // проверяем есть ли коллизия в желаемой точке

    AABB testAABB;
    testAABB = *aabb;
    aabb_shiftY(&testAABB, intendedDeltaY);

    if (!collision_searchTileCollision(testAABB, SOLID_TILE_INDEX)) {
        
        // если нет, выходим из функции

        return intendedDeltaY;

    } else {

        // если есть, проверяем не находится ли она на следующем пикселе

        testAABB = *aabb;
        aabb_shiftY(&testAABB, stepY);

        // если да, достаточно просто вернуть 0

        if (collision_searchTileCollision(testAABB, SOLID_TILE_INDEX)) {
            return 0;
        }

    }
    
    // проверяем есть ли коллизия пошагово
    
    // остальные переменные
    testAABB = *aabb;
    s16 actualDeltaY = 0;
    s16 distanceY = abs(intendedDeltaY);
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
        movedPixels = actualDeltaY - stepY; // Остановка на 1 пиксель раньше (работает для + и -)
    } else {
        movedPixels = intendedDeltaY;
    }

    return movedPixels;
}