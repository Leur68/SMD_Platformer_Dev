#include <global.h>

typedef struct {
    AxisLine_u16 x;
    AxisLine_u16 y;
    AxisLine_u16 tileX;
    AxisLine_u16 tileY;
} AABB;

bool aabb_intersects(AABB aabb1, AABB aabb2);
void aabb_shift(AABB *aabb, s16 shiftX, s16 shiftY);
void aabb_shiftX(AABB *aabb, s16 shiftX);
void aabb_shiftY(AABB *aabb, s16 shiftY);
void aabb_set(AABB *aabb, u16 x, u16 y);