#include "../inc/global.h"

bool aabb_intersects(AABB aabb1, AABB aabb2) {
    return (aabb1.x.max >= aabb2.x.min && aabb2.x.max >= aabb1.x.min) && (aabb1.y.max >= aabb2.y.min && aabb2.y.max >= aabb1.y.min);
}

void aabb_shift(AABB* aabb, Vect2D_s16 shift) {
    aabb->x.min = aabb->x.min + shift.x;
    aabb->x.max = aabb->x.max + shift.x;
    aabb->y.min = aabb->y.min + shift.y;
    aabb->y.max = aabb->y.max + shift.y;
    aabb_initTileIndexes(aabb);
}

void aabb_set(AABB* aabb, Vect2D_u16 pos) {
    aabb->x.min = pos.x;
    aabb->x.max = pos.x + PLAYER_WIDTH;
    aabb->y.min = pos.y;
    aabb->y.max = pos.y + PLAYER_HEIGHT;
    aabb_initTileIndexes(aabb);
}

AABB aabb_getTopAABB(AABB aabb) {
    AABB aabbTop;
    aabbTop.x = aabb.x;
    aabbTop.xTiles = aabb.xTiles;

    aabbTop.y.min = aabb.y.min - 8;
    aabbTop.y.max = aabb.y.min;

    aabbTop.yTiles.min = aabbTop.y.min >> 3;
    aabbTop.yTiles.max = aabb.yTiles.min;

    if (!math_isMultipleOfEight(aabbTop.y.min)) {
        aabbTop.yTiles.min += 1;
    }
    if (!math_isMultipleOfEight(aabbTop.y.max)) {
        aabbTop.yTiles.max += 1;
    }
    if (!math_isMultipleOfEight(aabbTop.x.max)) {
        aabbTop.xTiles.max += 1;
    }

    return aabbTop;
}

AABB aabb_getBottomAABB(AABB aabb) {
    AABB aabbBottom;
    aabbBottom.x = aabb.x;
    aabbBottom.xTiles = aabb.xTiles;

    aabbBottom.y.min = aabb.y.max;
    aabbBottom.y.max = aabb.y.max + 8;
    
    aabbBottom.yTiles.min = aabb.yTiles.max;
    aabbBottom.yTiles.max = aabbBottom.y.max >> 3;

    if (!math_isMultipleOfEight(aabbBottom.x.max)) {
        aabbBottom.xTiles.max += 1;
    }

    return aabbBottom;
}

AABB aabb_getLeftAABB(AABB aabb) {
    AABB aabbLeft;
    aabbLeft.y = aabb.y;
    aabbLeft.yTiles = aabb.yTiles;

    aabbLeft.x.min = aabb.x.min - 8;
    aabbLeft.x.max = aabb.x.min;
    
    aabbLeft.xTiles.min = aabbLeft.x.min >> 3;
    aabbLeft.xTiles.max = aabb.xTiles.min;

    if (!math_isMultipleOfEight(aabbLeft.x.min)) {
        aabbLeft.xTiles.min += 1;
    }
    if (!math_isMultipleOfEight(aabbLeft.x.max)) {
        aabbLeft.xTiles.max += 1;
    }
    if (!math_isMultipleOfEight(aabbLeft.y.max)) {
        aabbLeft.yTiles.max += 1;
    }

    return aabbLeft;
}

AABB aabb_getRightAABB(AABB aabb) {
    AABB aabbRight;
    aabbRight.y = aabb.y;
    aabbRight.yTiles = aabb.yTiles;

    aabbRight.x.min = aabb.x.max;
    aabbRight.x.max = aabb.x.max + 8;
    
    aabbRight.xTiles.min = aabb.xTiles.max;
    aabbRight.xTiles.max = aabbRight.x.max >> 3;

    if (!math_isMultipleOfEight(aabbRight.y.max)) {
        aabbRight.yTiles.max += 1;
    }

    return aabbRight;
}

void aabb_initTileIndexes(AABB* aabb) {
    aabb->xTiles.min = aabb->x.min >> 3;
    aabb->xTiles.max = aabb->x.max >> 3;
    aabb->yTiles.min = aabb->y.min >> 3;
    aabb->yTiles.max = aabb->y.max >> 3;
}