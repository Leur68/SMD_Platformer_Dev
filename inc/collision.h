#include <global.h>

AABB collision_checkMapArea(AABB aabb);
u8 collision_getIntersectionLen(AxisLine_u16 a, AxisLine_u16 b);
u8 collision_getTileIndex(u16 xTile, u16 yTile);
bool collision_isTileSolid(u16 xTile, u16 yTile);
void collision_check(AABB aabb, u8 direction, u8* left, u8* right, u8* top, u8* bottom);
bool collision_checkMovingTileIndex(u8 index, u8 middleIndex);
s16 collision_getMovingTileShift(u16 xTile, u16 yTile, u8 middleIndex);