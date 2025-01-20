#include "../inc/global.h"

u8 collision_getTileIndex(u16 xTile, u16 yTile) {
    if (xTile < 0 || yTile < 0 || xTile >= mapWTiles || yTile >= mapHTiles) return true;
    return mapPointerGet(collisionsMap, xTile, yTile);
}

bool collision_isTileSolid(u16 xTile, u16 yTile) {
    u8 index = collision_getTileIndex(xTile, yTile);
    return index == SOLID_TILE_INDEX;
}

bool collision_checkMapArea(AABB aabb, AABB* result) {
    u16  xMin   = aabb.tileX.max;   // Minimum x and y tile values
    u16  yMin   = aabb.tileY.max;
    u16  xMax   = aabb.tileX.min;   // Maximum x and y tile values
    u16  yMax   = aabb.tileY.min;
    bool exists = false;

    // Iterate through each tile in the specified area
    for (u16 currYTile = aabb.tileY.min; currYTile < aabb.tileY.max; currYTile++) {
        for (u16 currXTile = aabb.tileX.min; currXTile < aabb.tileX.max; currXTile++) {
            // Check if the tile is solid
            if (collision_isTileSolid(currXTile, currYTile)) {
                // Determine the minimum and maximum tile coordinates
                if (currXTile < xMin) xMin = currXTile;
                if (currYTile < yMin) yMin = currYTile;
                if (currXTile > xMax) xMax = currXTile;
                if (currYTile > yMax) yMax = currYTile;
                
                exists = true;
            }
        }
    }

    // If collisions are found, calculate the AABB
    if (exists) {
        result->tileX.min = xMin;
        result->tileY.min = yMin;
        result->tileX.max = xMax;
        result->tileY.max = yMax;

        // Convert tile coordinates to pixels
        result->x.min = xMin << 3;  // Multiply by 8 using bitwise shift
        result->y.min = yMin << 3;
        result->x.max = (xMax << 3) + 8;
        result->y.max = (yMax << 3) + 8;
    }

    return exists;
}

void collision_check(AABB aabb, u8 direction, u8* left, u8* right, u8* top, u8* bottom) {

    *left = *right = *top = *bottom = 0;

    // Get the AABB for collision checking, then filter out only the solid tiles on the map.
    // These tiles are those that are directly adjacent to or inside the corresponding sides of the player's AABB.
    AABB aabbLeft;
    AABB aabbTop;
    AABB aabbRight;
    AABB aabbBottom;
    u8 h;
    u8 v;

    switch (direction) {
        // Moving left only
        case DIRECTION_LEFT: 
            // Check collisions in the collision map
            aabbLeft = aabb_getLeftAABB(aabb);
        
            if (collision_checkMapArea(aabbLeft, &aabbLeft)) {
                *left = aabbLeft.x.max - aabb.x.min + 1;
            }
            // Check collisions with objects
            if (collidedObject != NULL && collidedObject->globalAABB.x.max < aabb.x.max) {
                h = collision_getIntersectionLen(collidedObject->globalAABB.x, aabb.x) + 1;
                v = collision_getIntersectionLen(collidedObject->globalAABB.y, aabb.y) + 1;

                if (v > h) {
                    // If the collision occurs along the player's movement direction, 
                    // the collision is determined along the player's axis of movement.
                    *left = h;
                } else if (h > v) {
                    // If the collision occurs perpendicular to the player's movement direction,
                    // the collision is determined along the axis of the object causing the collision.
                    switch (collidedObject->facingDirection) {
                        case DIRECTION_UP:
                            *bottom = h;
                            break;
                        case DIRECTION_DOWN:
                            *top = h;
                            break;
                    }
                }
            }
            break;
        // Moving right only
        case DIRECTION_RIGHT:
            aabbRight = aabb_getRightAABB(aabb);
            // Check collisions in the collision map
            if (collision_checkMapArea(aabbRight, &aabbRight)) {
                *right = aabb.x.max - aabbRight.x.min + 1;
            }
            // Check collisions with objects
            if (collidedObject != NULL && collidedObject->globalAABB.x.min > aabb.x.min) {
                h = collision_getIntersectionLen(collidedObject->globalAABB.x, aabb.x) + 1;
                v = collision_getIntersectionLen(collidedObject->globalAABB.y, aabb.y) + 1;

                if (v > h) {
                    // If the collision occurs along the player's movement direction, 
                    // the collision is determined along the player's axis of movement.
                    *right = h;
                } else if (h > v) {
                    // If the collision occurs perpendicular to the player's movement direction,
                    // the collision is determined along the axis of the object causing the collision.
                    switch (collidedObject->facingDirection) {
                        case DIRECTION_UP:
                            *bottom = h;
                            break;
                        case DIRECTION_DOWN:
                            *top = h;
                            break;
                    }
                }
            }
            break;
        // Moving up only
        case DIRECTION_UP:
            aabbTop = aabb_getTopAABB(aabb);
            // Check collisions in the collision map
            if (collision_checkMapArea(aabbTop, &aabbTop)) {
                *top = aabbTop.y.max - aabb.y.min + 1;
            }
            // Check collisions with objects
            if (collidedObject != NULL && collidedObject->globalAABB.y.max < aabb.y.max) {
                h = collision_getIntersectionLen(collidedObject->globalAABB.x, aabb.x) + 1;
                v = collision_getIntersectionLen(collidedObject->globalAABB.y, aabb.y) + 1;

                if (h > v) {
                    // If the collision occurs along the player's movement direction, 
                    // the collision is determined along the player's axis of movement.
                    *top = v;
                } else if (v > h) {
                    // If the collision occurs perpendicular to the player's movement direction,
                    // the collision is determined along the axis of the object causing the collision.
                    switch (collidedObject->facingDirection) {
                        case DIRECTION_LEFT:
                            *right = h;
                            break;
                        case DIRECTION_RIGHT:
                            *left = h;
                            break;
                    }
                }
            }
            break;
        // Moving down only
        case DIRECTION_DOWN:
            aabbBottom = aabb_getBottomAABB(aabb);
            // Check collisions in the collision map
            if (collision_checkMapArea(aabbBottom, &aabbBottom)) {
                *bottom = aabb.y.max - aabbBottom.y.min + 1;
            }
            // Check collisions with objects
            if (collidedObject != NULL && collidedObject->globalAABB.y.min > aabb.y.min) {
                h = collision_getIntersectionLen(collidedObject->globalAABB.x, aabb.x) + 1;
                v = collision_getIntersectionLen(collidedObject->globalAABB.y, aabb.y) + 1;

                if (h > v) {
                    // If the collision occurs along the player's movement direction, 
                    // the collision is determined along the player's axis of movement.
                    *bottom = v;
                } else if (v > h) {
                    // If the collision occurs perpendicular to the player's movement direction,
                    // the collision is determined along the axis of the object causing the collision.
                    switch (collidedObject->facingDirection) {
                        case DIRECTION_LEFT:
                            *right = h;
                            break;
                        case DIRECTION_RIGHT:
                            *left = h;
                            break;
                    }
                }
            }
            break;
        // Moving up-left
        case DIRECTION_LEFT_UP:
            // Check collisions in the collision map
            aabbLeft = aabb_getLeftAABB(aabb);
            aabbTop = aabb_getTopAABB(aabb);

            if (collision_checkMapArea(aabbLeft, &aabbLeft)) {
                u8 h = collision_getIntersectionLen(aabbLeft.x, aabb.x) + 1;
                u8 v = collision_getIntersectionLen(aabbLeft.y, aabb.y) + 1;

                if (v > h) {
                    *left = h;
                } else if (v == h) {
                    *left = h;
                    *top = v;
                }
            }
            if (collision_checkMapArea(aabbTop, &aabbTop)) {
                u8 h = collision_getIntersectionLen(aabbTop.x, aabb.x) + 1;
                u8 v = collision_getIntersectionLen(aabbTop.y, aabb.y) + 1;

                if (h > v) {
                    *top = v;
                } else if (v == h) {
                    *left = h;
                    *top = v;
                }
            }
            // Check collisions with objects
            if (collidedObject != NULL && (collidedObject->globalAABB.x.max < aabb.x.max || collidedObject->globalAABB.y.max < aabb.y.max)) {
                h = collision_getIntersectionLen(collidedObject->globalAABB.x, aabb.x) + 1;
                v = collision_getIntersectionLen(collidedObject->globalAABB.y, aabb.y) + 1;

                if (v > h) {
                    *left = h;
                } else if (h > v) {
                    *top = v;
                } else if (v == h) {
                    *left = h;
                    *top = v;
                }
            }
            break;
        // Moving up-right
        case DIRECTION_RIGHT_UP:
            // Check collisions in the collision map
            aabbRight = aabb_getRightAABB(aabb);
            aabbTop = aabb_getTopAABB(aabb);

            if (collision_checkMapArea(aabbRight, &aabbRight)) {
                u8 h = collision_getIntersectionLen(aabbRight.x, aabb.x) + 1;
                u8 v = collision_getIntersectionLen(aabbRight.y, aabb.y) + 1;

                if (v > h) {
                    *right = h;
                } else if (v == h) {
                    *right = h;
                    *top = v;
                }
            }
            if (collision_checkMapArea(aabbTop, &aabbTop)) {
                u8 h = collision_getIntersectionLen(aabbTop.x, aabb.x) + 1;
                u8 v = collision_getIntersectionLen(aabbTop.y, aabb.y) + 1;

                if (h > v) {
                    *top = v;
                } else if (v == h) {
                    *right = h;
                    *top = v;
                }
            }
            // Check collisions with objects
            if (collidedObject != NULL && (collidedObject->globalAABB.x.min > aabb.x.min || collidedObject->globalAABB.y.max < aabb.y.max)) {
                h = collision_getIntersectionLen(collidedObject->globalAABB.x, aabb.x) + 1;
                v = collision_getIntersectionLen(collidedObject->globalAABB.y, aabb.y) + 1;

                if (v > h) {
                    *right = h;
                } else if (h > v) {
                    *top = v;
                } else if (v == h) {
                    *right = h;
                    *top = v;
                }
            }
            break;
        // Moving down-left
        case DIRECTION_LEFT_DOWN:
            // Check collisions in the collision map
            aabbLeft = aabb_getLeftAABB(aabb);
            aabbBottom = aabb_getBottomAABB(aabb);

            if (collision_checkMapArea(aabbLeft, &aabbLeft)) {
                u8 h = collision_getIntersectionLen(aabbLeft.x, aabb.x) + 1;
                u8 v = collision_getIntersectionLen(aabbLeft.y, aabb.y) + 1;
            
                if (v > h) {
                    *left = h;
                } else if (v == h) {
                    *left = h;
                    *bottom = v;
                }
            }
            if (collision_checkMapArea(aabbBottom, &aabbBottom)) {
                u8 h = collision_getIntersectionLen(aabbBottom.x, aabb.x) + 1;
                u8 v = collision_getIntersectionLen(aabbBottom.y, aabb.y) + 1;
            
                if (h > v) {
                    *bottom = v;
                } else if (v == h) {
                    *left = h;
                    *bottom = v;
                }
            }
            // Check collisions with objects
            if (collidedObject != NULL && (collidedObject->globalAABB.x.max < aabb.x.max || collidedObject->globalAABB.y.min > aabb.y.min)) {
                h = collision_getIntersectionLen(collidedObject->globalAABB.x, aabb.x) + 1;
                v = collision_getIntersectionLen(collidedObject->globalAABB.y, aabb.y) + 1;
            
                if (v > h) {
                    *left = h;
                } else if (h > v) {
                    *bottom = v;
                } else if (v == h) {
                    *left = h;
                    *bottom = v;
                }
            }
            break;
        // Moving down-right
        case DIRECTION_RIGHT_DOWN:
            // Check collisions in the collision map
            aabbRight = aabb_getRightAABB(aabb);
            aabbBottom = aabb_getBottomAABB(aabb);

            if (collision_checkMapArea(aabbRight, &aabbRight)) {
                u8 h = collision_getIntersectionLen(aabbRight.x, aabb.x) + 1;
                u8 v = collision_getIntersectionLen(aabbRight.y, aabb.y) + 1;

                if (v > h) {
                    *right = h;
                } else if (v == h) {
                    *right = h;
                    *bottom = v;
                }
            }
            if (collision_checkMapArea(aabbBottom, &aabbBottom)) {
                u8 h = collision_getIntersectionLen(aabbBottom.x, aabb.x) + 1;
                u8 v = collision_getIntersectionLen(aabbBottom.y, aabb.y) + 1;

                if (h > v) {
                    *bottom = v;
                } else if (v == h) {
                    *right = h;
                    *bottom = v;
                }
            }
            // Check collisions with objects
            if (collidedObject != NULL && (collidedObject->globalAABB.x.min > aabb.x.min || collidedObject->globalAABB.y.min > aabb.y.min)) {
                h = collision_getIntersectionLen(collidedObject->globalAABB.x, aabb.x) + 1;
                v = collision_getIntersectionLen(collidedObject->globalAABB.y, aabb.y) + 1;

                if (v > h) {
                    *right = h;
                } else if (h > v) {
                    *bottom = v;
                } else if (v == h) {
                    *right = h;
                    *bottom = v;
                }
            }
    }
}

u8 collision_getIntersectionLen(AxisLine_u16 a, AxisLine_u16 b) {
    u16 start = a.min > b.min ? a.min : b.min; // The larger of the two minimum values
    u16 end = a.max < b.max ? a.max : b.max;   // The smaller of the two maximum values

    u8 len = end - start; // Calculate the length of the intersection
    return len > 0 ? len : 0; // If the intersection length is negative, there is no intersection
}