#include "../inc/global.h"

bool aabb_intersects(AABB aabb1, AABB aabb2)
{
    return (aabb1.x.max >= aabb2.x.min && aabb2.x.max >= aabb1.x.min) && (aabb1.y.max >= aabb2.y.min && aabb2.y.max >= aabb1.y.min);
}

void aabb_shift(AABB *aabb, Vect2D_s16 shift)
{
    aabb->x.min = aabb->x.min + shift.x;
    aabb->x.max = aabb->x.max + shift.x;
    aabb->y.min = aabb->y.min + shift.y;
    aabb->y.max = aabb->y.max + shift.y;
    aabb_updateTiles(aabb);
}

void aabb_set(AABB *aabb, Vect2D_u16 pos)
{
    aabb->x.min = pos.x;
    aabb->x.max = pos.x + PLAYER_WIDTH;
    aabb->y.min = pos.y;
    aabb->y.max = pos.y + PLAYER_HEIGHT;
    aabb_updateTiles(aabb);
}

AABB aabb_getTopAABB(AABB aabb)
{
    AABB aabbTop;
    aabbTop.x = aabb.x;
    aabbTop.tileX = aabb.tileX;

    aabbTop.y.min = aabb.y.min - 8;
    aabbTop.y.max = aabb.y.min;

    aabbTop.tileY.min = aabbTop.y.min >> 3;
    aabbTop.tileY.max = aabb.tileY.min;

    if (!math_isMultipleOfEight(aabbTop.y.min))
    {
        aabbTop.tileY.min += 1;
    }
    if (!math_isMultipleOfEight(aabbTop.y.max))
    {
        aabbTop.tileY.max += 1;
    }
    if (!math_isMultipleOfEight(aabbTop.x.max))
    {
        aabbTop.tileX.max += 1;
    }

    return aabbTop;
}

AABB aabb_getBottomAABB(AABB aabb)
{
    AABB aabbBottom;
    aabbBottom.x = aabb.x;
    aabbBottom.tileX = aabb.tileX;

    aabbBottom.y.min = aabb.y.max;
    aabbBottom.y.max = aabb.y.max + 8;

    aabbBottom.tileY.min = aabb.tileY.max;
    aabbBottom.tileY.max = aabbBottom.y.max >> 3;

    if (!math_isMultipleOfEight(aabbBottom.x.max))
    {
        aabbBottom.tileX.max += 1;
    }

    return aabbBottom;
}

AABB aabb_getLeftAABB(AABB aabb)
{
    AABB aabbLeft;
    aabbLeft.y = aabb.y;
    aabbLeft.tileY = aabb.tileY;

    aabbLeft.x.min = aabb.x.min - 8;
    aabbLeft.x.max = aabb.x.min;

    aabbLeft.tileX.min = aabbLeft.x.min >> 3;
    aabbLeft.tileX.max = aabb.tileX.min;

    if (!math_isMultipleOfEight(aabbLeft.x.min))
    {
        aabbLeft.tileX.min += 1;
    }
    if (!math_isMultipleOfEight(aabbLeft.x.max))
    {
        aabbLeft.tileX.max += 1;
    }
    if (!math_isMultipleOfEight(aabbLeft.y.max))
    {
        aabbLeft.tileY.max += 1;
    }

    return aabbLeft;
}

AABB aabb_getRightAABB(AABB aabb)
{
    AABB aabbRight;
    aabbRight.y = aabb.y;
    aabbRight.tileY = aabb.tileY;

    aabbRight.x.min = aabb.x.max;
    aabbRight.x.max = aabb.x.max + 8;

    aabbRight.tileX.min = aabb.tileX.max;
    aabbRight.tileX.max = aabbRight.x.max >> 3;

    if (!math_isMultipleOfEight(aabbRight.y.max))
    {
        aabbRight.tileY.max += 1;
    }

    return aabbRight;
}

void aabb_updateTiles(AABB *aabb)
{
    aabb->tileX.min = aabb->x.min >> 3;
    aabb->tileX.max = aabb->x.max >> 3;
    aabb->tileY.min = aabb->y.min >> 3;
    aabb->tileY.max = aabb->y.max >> 3;
}