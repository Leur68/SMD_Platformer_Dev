#include <global.h>

typedef struct {
    AxisLine_u16 x;
    AxisLine_u16 y;
    AxisLine_u16 xTiles;
    AxisLine_u16 yTiles;
} AABB;

bool aabb_intersects(AABB aabb1, AABB aabb2);
AABB aabb_getTopAABB(AABB aabb);
AABB aabb_getBottomAABB(AABB aabb);
AABB aabb_getLeftAABB(AABB aabb);
AABB aabb_getRightAABB(AABB aabb);
void aabb_initTileIndexes(AABB* aabb);
void aabb_shift(AABB* aabb, Vect2D_s16 shift);
void aabb_set(AABB* aabb, Vect2D_u16 pos);