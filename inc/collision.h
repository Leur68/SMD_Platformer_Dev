#include <global.h>

#define LEFT_BIT_SHIFT     0
#define RIGHT_BIT_SHIFT    3
#define TOP_BIT_SHIFT      6
#define BOTTOM_BIT_SHIFT   9
#define GROUND_BIT_POS     12

#define GET_LEFT_COLLISION(collider)            (collider->groundCollisionData & 0x7)
#define GET_RIGHT_COLLISION(collider)           ((collider->groundCollisionData >> RIGHT_BIT_SHIFT) & 0x7)
#define GET_TOP_COLLISION(collider)             ((collider->groundCollisionData >> TOP_BIT_SHIFT) & 0x7)
#define GET_BOTTOM_COLLISION(collider)          ((collider->groundCollisionData >> BOTTOM_BIT_SHIFT) & 0x7)
#define HAS_ANY_COLLISION(collider)             ((collider->groundCollisionData & 0x0FFF) ? 1 : 0)
#define HAS_GROUND_COLLISION(collider)          ((collider->groundCollisionData >> GROUND_BIT_POS) & 1)
#define HAS_TILE_COLLISION(collider, tileIndex)  ((collider->tileCollisionFlags >> tileIndex) & 1)

// Contains minimal information required to calculate and determine collisions
typedef struct {
    AABB globalAABB; // Object's global axis-aligned bounding box for collision detection
    u8 facingDirection; // Direction the object is facing
    u16 groundCollisionData;
    u16 tileCollisionFlags;
} Collider;

bool collision_checkMapArea(AABB targetAABB, AABB *collidingTilesAABB, u16 *tileCollisionFlags);
s16 collision_getIntersectionLen(AxisLine_u16 a, AxisLine_u16 b);
u8 collision_getTileIndex(u16 xTile, u16 yTile);
void collision_check(Collider *collider);