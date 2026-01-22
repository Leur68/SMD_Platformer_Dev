#include "global.h"

bool aabb_intersects(AABB aabb1, AABB aabb2) {
    return (aabb1.x.max >= aabb2.x.min && aabb2.x.max >= aabb1.x.min) && (aabb1.y.max >= aabb2.y.min && aabb2.y.max >= aabb1.y.min);
}

void aabb_shiftX(AABB *aabb, s16 shiftX) {
    aabb->x.min = aabb->x.min + shiftX;
    aabb->x.max = aabb->x.max + shiftX;
    aabb->tileX.min = aabb->x.min >> 3;
    aabb->tileX.max = aabb->x.max >> 3;
}

void aabb_shiftY(AABB *aabb, s16 shiftY) {
    aabb->y.min = aabb->y.min + shiftY;
    aabb->y.max = aabb->y.max + shiftY;
    aabb->tileY.min = aabb->y.min >> 3;
    aabb->tileY.max = aabb->y.max >> 3;
}

void aabb_shift(AABB *aabb, s16 shiftX, s16 shiftY) {
    aabb->x.min = aabb->x.min + shiftX;
    aabb->x.max = aabb->x.max + shiftX;
    aabb->y.min = aabb->y.min + shiftY;
    aabb->y.max = aabb->y.max + shiftY;
    aabb->tileX.min = aabb->x.min >> 3;
    aabb->tileX.max = aabb->x.max >> 3;
    aabb->tileY.min = aabb->y.min >> 3;
    aabb->tileY.max = aabb->y.max >> 3;
}

void aabb_set(AABB *aabb, u16 x, u16 y) {
    aabb->x.min = x;
    aabb->x.max = x + PLAYER_WIDTH;
    aabb->y.min = y;
    aabb->y.max = y + PLAYER_HEIGHT;
    aabb->tileX.min = aabb->x.min >> 3;
    aabb->tileX.max = aabb->x.max >> 3;
    aabb->tileY.min = aabb->y.min >> 3;
    aabb->tileY.max = aabb->y.max >> 3;
}