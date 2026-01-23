#include <global.h>

#define LEFT_BIT_SHIFT     0
#define RIGHT_BIT_SHIFT    1
#define TOP_BIT_SHIFT      2
#define BOTTOM_BIT_SHIFT   3
#define GROUND_BIT_POS     4

#define GET_LEFT_COLLISION(collider)            (collider->groundCollisionData & 1)
#define GET_RIGHT_COLLISION(collider)           ((collider->groundCollisionData >> RIGHT_BIT_SHIFT) & 1)
#define GET_TOP_COLLISION(collider)             ((collider->groundCollisionData >> TOP_BIT_SHIFT) & 1)
#define GET_BOTTOM_COLLISION(collider)          ((collider->groundCollisionData >> BOTTOM_BIT_SHIFT) & 1)
#define HAS_ANY_COLLISION(collider)             ((collider->groundCollisionData & 0x0FFF) ? 1 : 0)
#define HAS_GROUND_COLLISION(collider)          ((collider->groundCollisionData >> GROUND_BIT_POS) & 1)
#define HAS_TILE_COLLISION(flags, tileIndex)    ((flags >> tileIndex) & 1)

// Contains minimal information required to calculate and determine collisions
typedef struct {
    AABB globalAABB; // Object's global axis-aligned bounding box for collision detection

    u16 groundCollisionData;


    // Bits:  15 | 14 | 13 | 12 | 11 | 10 |  9 |  8 |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0
    //        -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | --
    //       T15 |T14 |T13 |T12 |T11 |T10 | T9 | T8 | T7 | T6 | T5 | T4 | T3 | T2 | T1 | T0
    u16 tileCollisionFlags;  // 
} Collider;

Collider *allocCollider();
void collider_init(Collider* collider, u16 x, u16 y);
u8 collision_getTileIndex(u16 xTile, u16 yTile);
bool collision_checkTileCollisions(AABB aabb, u16 *tileCollisionFlags);
bool collision_searchTileCollision(AABB aabb, u8 tileIndex);
s16 collision_computeDeltaX(AABB* aabb, s16 intendedDeltaX);
s16 collision_computeDeltaY(AABB* aabb, s16 intendedDeltaY);