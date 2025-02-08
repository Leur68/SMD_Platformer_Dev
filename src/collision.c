#include "../inc/global.h"

u8 collision_getTileIndex(u16 xTile, u16 yTile) {
    if (xTile < 0 || yTile < 0 || xTile >= mapWTiles || yTile >= mapHTiles)
        return true;
    return mapPointerGet(collisionsMap, xTile, yTile);
}

bool collision_checkMapArea(AABB targetAABB, AABB *collidingTilesAABB, u16 *tileCollisionFlags) {
    u16 xMin = targetAABB.tileX.max; // Minimum x and y tile values
    u16 yMin = targetAABB.tileY.max;
    u16 xMax = targetAABB.tileX.min; // Maximum x and y tile values
    u16 yMax = targetAABB.tileY.min;
    bool exists = false;

    // Iterate through each tile in the specified area
    for (u16 currYTile = targetAABB.tileY.min; currYTile < targetAABB.tileY.max; currYTile++) {
        for (u16 currXTile = targetAABB.tileX.min; currXTile < targetAABB.tileX.max; currXTile++) {
            // Check if the tile is solid
            u8 tileIndex = collision_getTileIndex(currXTile, currYTile);
            if (tileIndex == SOLID_TILE_INDEX) {
                // Determine the minimum and maximum tile coordinates
                if (currXTile < xMin) xMin = currXTile;
                if (currYTile < yMin) yMin = currYTile;
                if (currXTile > xMax) xMax = currXTile;
                if (currYTile > yMax) yMax = currYTile;

                exists = true;
            } else {
                setBit(tileCollisionFlags, true, tileIndex);
            }
        }
    }

    // If collisions are found, calculate the AABB
    if (exists) {
        collidingTilesAABB->tileX.min = xMin;
        collidingTilesAABB->tileY.min = yMin;
        collidingTilesAABB->tileX.max = xMax;
        collidingTilesAABB->tileY.max = yMax;

        // Convert tile coordinates to pixels
        collidingTilesAABB->x.min = xMin << 3; // Multiply by 8 using bitwise shift
        collidingTilesAABB->y.min = yMin << 3;
        collidingTilesAABB->x.max = (xMax << 3) + 8;
        collidingTilesAABB->y.max = (yMax << 3) + 8;
    }

    return exists;
}

void collision_check(Collider *collider) {
    // Get the AABB for collision checking, then filter out only the solid tiles on the map.
    // These tiles are those that are directly adjacent to or inside the corresponding sides of the player's AABB.
    AABB aabbLeft;
    AABB aabbTop;
    AABB aabbRight;
    AABB aabbBottom;
    AABB objectAABB = collider->globalAABB;
    u16 *groundCollisionData = &collider->groundCollisionData;
    u16 *tileCollisionFlags = &collider->tileCollisionFlags;
    *groundCollisionData = 0;
    *tileCollisionFlags = 0;

    switch (collider->facingDirection) {
    // Moving left only
    case DIRECTION_LEFT:
        // Check collisions in the collision map
        aabbLeft = aabb_getLeftAABB(objectAABB);

        if (collision_checkMapArea(aabbLeft, &aabbLeft, tileCollisionFlags)) {
            setThreeBitField(groundCollisionData, aabbLeft.x.max - objectAABB.x.min + 1, LEFT_BIT_SHIFT);
            setBit(groundCollisionData, true, GROUND_BIT_POS);
        }
        break;
    // Moving right only
    case DIRECTION_RIGHT:
        aabbRight = aabb_getRightAABB(objectAABB);
        // Check collisions in the collision map
        if (collision_checkMapArea(aabbRight, &aabbRight, tileCollisionFlags)) {
            setThreeBitField(groundCollisionData, objectAABB.x.max - aabbRight.x.min + 1, RIGHT_BIT_SHIFT);
            setBit(groundCollisionData, true, GROUND_BIT_POS);
        }
        break;
    // Moving up only
    case DIRECTION_UP:
        aabbTop = aabb_getTopAABB(objectAABB);
        // Check collisions in the collision map
        if (collision_checkMapArea(aabbTop, &aabbTop, tileCollisionFlags)) {
            setThreeBitField(groundCollisionData, aabbTop.y.max - objectAABB.y.min + 1, TOP_BIT_SHIFT);
            setBit(groundCollisionData, true, GROUND_BIT_POS);
        }
        break;
    // Moving down only
    case DIRECTION_DOWN:
        aabbBottom = aabb_getBottomAABB(objectAABB);
        // Check collisions in the collision map
        if (collision_checkMapArea(aabbBottom, &aabbBottom, tileCollisionFlags)) {
            setThreeBitField(groundCollisionData, objectAABB.y.max - aabbBottom.y.min + 1, BOTTOM_BIT_SHIFT);
            setBit(groundCollisionData, true, GROUND_BIT_POS);
        }
        break;
    // Moving up-left
    case DIRECTION_LEFT_UP:
        // Check collisions in the collision map
        aabbLeft = aabb_getLeftAABB(objectAABB);
        aabbTop = aabb_getTopAABB(objectAABB);

        if (collision_checkMapArea(aabbLeft, &aabbLeft, tileCollisionFlags)) {
            s16 h = collision_getIntersectionLen(aabbLeft.x, objectAABB.x) + 1;
            s16 v = collision_getIntersectionLen(aabbLeft.y, objectAABB.y) + 1;

            if (v > h) {
                setThreeBitField(groundCollisionData, h, LEFT_BIT_SHIFT);
            } else if (v == h) {
                setThreeBitField(groundCollisionData, h, LEFT_BIT_SHIFT);
                setThreeBitField(groundCollisionData, v, TOP_BIT_SHIFT);
            }
            setBit(groundCollisionData, true, GROUND_BIT_POS);
        }
        if (collision_checkMapArea(aabbTop, &aabbTop, tileCollisionFlags)) {
            s16 h = collision_getIntersectionLen(aabbTop.x, objectAABB.x) + 1;
            s16 v = collision_getIntersectionLen(aabbTop.y, objectAABB.y) + 1;

            if (h > v) {
                setThreeBitField(groundCollisionData, v, TOP_BIT_SHIFT);
            } else if (v == h) {
                setThreeBitField(groundCollisionData, h, LEFT_BIT_SHIFT);
                setThreeBitField(groundCollisionData, v, TOP_BIT_SHIFT);
            }
            setBit(groundCollisionData, true, GROUND_BIT_POS);
        }
        break;
    // Moving up-right
    case DIRECTION_RIGHT_UP:
        // Check collisions in the collision map
        aabbRight = aabb_getRightAABB(objectAABB);
        aabbTop = aabb_getTopAABB(objectAABB);

        if (collision_checkMapArea(aabbRight, &aabbRight, tileCollisionFlags)) {
            s16 h = collision_getIntersectionLen(aabbRight.x, objectAABB.x) + 1;
            s16 v = collision_getIntersectionLen(aabbRight.y, objectAABB.y) + 1;

            if (v > h) {
                setThreeBitField(groundCollisionData, h, RIGHT_BIT_SHIFT);
            } else if (v == h) {
                setThreeBitField(groundCollisionData, h, RIGHT_BIT_SHIFT);
                setThreeBitField(groundCollisionData, v, TOP_BIT_SHIFT);
            }
            setBit(groundCollisionData, true, GROUND_BIT_POS);
        }
        if (collision_checkMapArea(aabbTop, &aabbTop, tileCollisionFlags)) {
            s16 h = collision_getIntersectionLen(aabbTop.x, objectAABB.x) + 1;
            s16 v = collision_getIntersectionLen(aabbTop.y, objectAABB.y) + 1;

            if (h > v) {
                setThreeBitField(groundCollisionData, v, TOP_BIT_SHIFT);
            } else if (v == h) {
                setThreeBitField(groundCollisionData, h, RIGHT_BIT_SHIFT);
                setThreeBitField(groundCollisionData, v, TOP_BIT_SHIFT);
            }
            setBit(groundCollisionData, true, GROUND_BIT_POS);
        }
        break;
    // Moving down-left
    case DIRECTION_LEFT_DOWN:
        // Check collisions in the collision map
        aabbLeft = aabb_getLeftAABB(objectAABB);
        aabbBottom = aabb_getBottomAABB(objectAABB);

        if (collision_checkMapArea(aabbLeft, &aabbLeft, tileCollisionFlags)) {
            s16 h = collision_getIntersectionLen(aabbLeft.x, objectAABB.x) + 1;
            s16 v = collision_getIntersectionLen(aabbLeft.y, objectAABB.y) + 1;

            if (v > h) {
                setThreeBitField(groundCollisionData, h, LEFT_BIT_SHIFT);
            } else if (v == h) {
                setThreeBitField(groundCollisionData, h, LEFT_BIT_SHIFT);
                setThreeBitField(groundCollisionData, v, BOTTOM_BIT_SHIFT);
            }
            setBit(groundCollisionData, true, GROUND_BIT_POS);
        }
        if (collision_checkMapArea(aabbBottom, &aabbBottom, tileCollisionFlags)) {
            s16 h = collision_getIntersectionLen(aabbBottom.x, objectAABB.x) + 1;
            s16 v = collision_getIntersectionLen(aabbBottom.y, objectAABB.y) + 1;

            if (h > v) {
                setThreeBitField(groundCollisionData, v, BOTTOM_BIT_SHIFT);
            } else if (v == h) {
                setThreeBitField(groundCollisionData, h, LEFT_BIT_SHIFT);
                setThreeBitField(groundCollisionData, v, BOTTOM_BIT_SHIFT);
            }
            setBit(groundCollisionData, true, GROUND_BIT_POS);
        }
        break;
    // Moving down-right
    case DIRECTION_RIGHT_DOWN:
        // Check collisions in the collision map
        aabbRight = aabb_getRightAABB(objectAABB);
        aabbBottom = aabb_getBottomAABB(objectAABB);

        if (collision_checkMapArea(aabbRight, &aabbRight, tileCollisionFlags)) {
            s16 h = collision_getIntersectionLen(aabbRight.x, objectAABB.x) + 1;
            s16 v = collision_getIntersectionLen(aabbRight.y, objectAABB.y) + 1;

            if (v > h) {
                setThreeBitField(groundCollisionData, h, RIGHT_BIT_SHIFT);
            } else if (v == h) {
                setThreeBitField(groundCollisionData, h, RIGHT_BIT_SHIFT);
                setThreeBitField(groundCollisionData, v, BOTTOM_BIT_SHIFT);
            }
            setBit(groundCollisionData, true, GROUND_BIT_POS);
        }
        if (collision_checkMapArea(aabbBottom, &aabbBottom, tileCollisionFlags)) {
            s16 h = collision_getIntersectionLen(aabbBottom.x, objectAABB.x) + 1;
            s16 v = collision_getIntersectionLen(aabbBottom.y, objectAABB.y) + 1;

            if (h > v) {
                setThreeBitField(groundCollisionData, v, BOTTOM_BIT_SHIFT);
            } else if (v == h) {
                setThreeBitField(groundCollisionData, h, RIGHT_BIT_SHIFT);
                setThreeBitField(groundCollisionData, v, BOTTOM_BIT_SHIFT);
            }
            setBit(groundCollisionData, true, GROUND_BIT_POS);
        }
    }

    // Check collisions with objects
    if (collidedObject != NULL) {

        s16 h = collision_getIntersectionLen(collidedObject->globalAABB.x, objectAABB.x) + 1;
        s16 v = collision_getIntersectionLen(collidedObject->globalAABB.y, objectAABB.y) + 1;
        u8 relativeDirection = aabb_getRelativePosition(objectAABB, collidedObject->globalAABB);   

        if (v > h) { 
            if (IS_ON_LEFT(relativeDirection)) { // check if collidedObject is on the left
                setThreeBitField(groundCollisionData, h, LEFT_BIT_SHIFT);
            } else if (IS_ON_RIGHT(relativeDirection)) { // check if collidedObject is on the right
                setThreeBitField(groundCollisionData, h, RIGHT_BIT_SHIFT);
            }
        } else if (h > v) {
            if (IS_ON_TOP(relativeDirection)) { // check if collidedObject is on the top
                setThreeBitField(groundCollisionData, v, TOP_BIT_SHIFT);
            } else if (IS_ON_BOTTOM(relativeDirection)) { // check if collidedObject is on the bottom
                setThreeBitField(groundCollisionData, v, BOTTOM_BIT_SHIFT);
            }
        } else if (v == h) {
            // We take into account the character's movement direction so that after the shift, v does not equal h.

            if (player->velocity.x > player->velocity.y || player->autoVelocity.x > player->autoVelocity.y) {
                if (IS_ON_LEFT(relativeDirection)) { // check if collidedObject is on the left
                    setThreeBitField(groundCollisionData, h, LEFT_BIT_SHIFT);
                } else if (IS_ON_RIGHT(relativeDirection)) { // check if collidedObject is on the right
                    setThreeBitField(groundCollisionData, h, RIGHT_BIT_SHIFT);
                }
            } else if (player->velocity.y > player->velocity.x || player->autoVelocity.y > player->autoVelocity.x) {
                if (IS_ON_TOP(relativeDirection)) { // check if collidedObject is on the top
                    setThreeBitField(groundCollisionData, v, TOP_BIT_SHIFT);
                } else if (IS_ON_BOTTOM(relativeDirection)) { // check if collidedObject is on the bottom
                    setThreeBitField(groundCollisionData, v, BOTTOM_BIT_SHIFT);
                }
            }
        }
    }
}

s16 collision_getIntersectionLen(AxisLine_u16 a, AxisLine_u16 b) {
    u16 start = a.min > b.min ? a.min : b.min; // The larger of the two minimum values
    u16 end = a.max < b.max ? a.max : b.max;   // The smaller of the two maximum values

    s16 len = end - start;     // Calculate the length of the intersection
    return len > 0 ? len : 0; // If the intersection length is negative, there is no intersection
}