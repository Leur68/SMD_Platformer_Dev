#include <global.h>

bool collision_checkMapArea(AABB aabb, AABB *result);
s16 collision_getIntersectionLen(AxisLine_u16 a, AxisLine_u16 b);
u8 collision_getTileIndex(u16 xTile, u16 yTile);
bool collision_isTileSolid(u16 xTile, u16 yTile);
void collision_check(AABB aabb, u8 direction, u8 *left, u8 *right, u8 *top, u8 *bottom, bool *ground);