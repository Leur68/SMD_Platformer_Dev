#include "global.h"

Player *allocPlayer() {
    return (Player *)MEM_alloc(sizeof(Player));
}

void player_init(u16 startX, u16 startY) {
    // Sprite
    player->sprite = SPR_addSpriteSafe(&player_sprite, startX, startY, TILE_ATTR(PLAYER_PALETTE, 0, false, true));
    SPR_setAlwaysOnTop(player->sprite);
    PAL_setPalette(PLAYER_PALETTE, player_sprite.palette->data, DMA);

    // Collider, position
    player->collider = allocCollider();
    collider_init(player->collider, startX, startY + mapMaxCameraPosY);

    player->posBuffer = (Vect2D_ff32){FASTFIX32(player->collider->globalAABB.x.min), FASTFIX32(player->collider->globalAABB.y.min)};

    // Movement
    player->velocity = (Vect2D_ff32){FASTFIX32(0), FASTFIX32(ACCELERATION)};
    player->autoVelocity = (Vect2D_ff32){FASTFIX32(0), FASTFIX32(0)};
    player->movedPixels = (Vect2D_s16){0, 0};

    // Flags
    player->isJumping = false;
    player->isFalling = false; 
    player->isClimbing = false;
    player->decelerating = false;

    // Timers
    player->coyoteTimer = 0;
    player->jumpTimer = 0;
    player->waterTimer = 0;
}

void player_update() {
    /////////////////////////////////////
    // Update properties before moving //
    /////////////////////////////////////

    // ?????????????????
    AABB checkAABB = player->collider->globalAABB;
    aabb_shiftY(&checkAABB, 1);
    bool bottom = collision_searchTileCollision(checkAABB, SOLID_TILE_INDEX);
    if (bottom) {
        setBit(&player->collider->groundCollisionData, BOTTOM_BIT_SHIFT);
    } else {
        clearBit(&player->collider->groundCollisionData, BOTTOM_BIT_SHIFT);
    }

    bool onStairs = HAS_TILE_COLLISION(player->collider->tileCollisionFlags, STAIRS_TILE_INDEX);

    // Handle deceleration when movement keys are released
    if (player->decelerating) {
        if (bottom) {
            if (player->velocity.x < FASTFIX32(0)) { // left
                player->velocity.x += FASTFIX32(DECELERATION);
                if (player->velocity.x > FASTFIX32(0)) {
                    player->velocity.x = FASTFIX32(0);
                }
            } else if (player->velocity.x > FASTFIX32(0)) { // right
                player->velocity.x -= FASTFIX32(DECELERATION);
                if (player->velocity.x < FASTFIX32(0)) {
                    player->velocity.x = FASTFIX32(0);
                }
            }

            if (player->velocity.x == FASTFIX32(0)) {
                player->decelerating = false;
            }
        } else {
            player->decelerating = false;
            player->velocity.x = FASTFIX32(0);
        }
    }

    if (player->isClimbing && !onStairs) {
        player->isClimbing = false;
    }

// Add gravity - проверяем РЕАЛЬНУЮ коллизию вниз, а не старую
#if (!DEBUG_FREE_MOVE_MODE)
    // Проверяем, есть ли что-то под ногами прямо сейчас
    //AABB checkAABB = player->collider->globalAABB;
    //aabb_shiftY(&checkAABB, 1);
    //bool hasGroundBelow = collision_searchTileCollision(checkAABB, SOLID_TILE_INDEX);
    
    if ((!bottom && !onStairs) || player->isJumping) {
        if (player->velocity.y < FASTFIX32(GRAVITY)) {
            player->velocity.y += FASTFIX32(GRAVITY_ACCELERATION);
        }
        if (player->velocity.y > FASTFIX32(GRAVITY)) {
            player->velocity.y = FASTFIX32(GRAVITY);
        }
    }
#endif

    if (player->velocity.x < FASTFIX32(0)) {

        SPR_setHFlip(player->sprite, false); // Update sprite's horizontal orientation

    } else if (player->velocity.x > FASTFIX32(0)) {

        SPR_setHFlip(player->sprite, true); // Update sprite's horizontal orientation

    }

    u8 lastBottom = bottom;
    bool lastInWater = HAS_TILE_COLLISION(player->collider->tileCollisionFlags, WATER_TILE_INDEX);

    ///////////////////////////////////////////
    // Move the player and handle collisions //
    ///////////////////////////////////////////

    player->movedPixels = (Vect2D_s16){0, 0};

    if (player->velocity.x != FASTFIX32(0) || player->autoVelocity.x != FASTFIX32(0)) {
        // расчет желаемого расстояния движения
        fastfix32 bufferX = (player->posBuffer.x + player->velocity.x + player->autoVelocity.x);
        s16 newPosX = FF32_toInt(FF32_int(bufferX));
        s16 intendedDeltaX = newPosX - player->collider->globalAABB.x.min;
        if (intendedDeltaX != 0) {

            player->movedPixels.x = collision_moveX(player->collider, intendedDeltaX);

            if (player->movedPixels.x != intendedDeltaX) {
                player->velocity.x = FASTFIX32(0);
                player->autoVelocity.x = FASTFIX32(0);
            }
            
            if (player->movedPixels.x != 0) {
                player->posBuffer.x += FASTFIX32(player->movedPixels.x);
            }
        }
    }

    if (player->velocity.y != FASTFIX32(0) || player->autoVelocity.y != FASTFIX32(0)) {
        // расчет желаемого расстояния движения
        fastfix32 bufferY = (player->posBuffer.y + player->velocity.y + player->autoVelocity.y);
        s16 newPosY = FF32_toInt(FF32_int(bufferY));
        s16 intendedDeltaY = newPosY - player->collider->globalAABB.y.min;
        if (intendedDeltaY != 0) {
            
            player->movedPixels.y = collision_moveY(player->collider, intendedDeltaY);

            if (player->movedPixels.y != intendedDeltaY) {
                player->velocity.y = FASTFIX32(0);
                player->autoVelocity.y = FASTFIX32(0);
            }

            if (player->movedPixels.y != 0) {
                player->posBuffer.y += FASTFIX32(player->movedPixels.y);
            }
        }
    }

    collision_checkTileCollisions(player->collider->globalAABB, &player->collider->tileCollisionFlags);


/*
    GameObject** objects = (GameObject**) POOL_getFirst(objectsPool);
    u16 num = POOL_getNumAllocated(objectsPool);
    while (num--) {
        GameObject *object = *objects++;

        bool intersects = aabb_intersects(player->collider->globalAABB, object->globalAABB);

        if (intersects) {
            collidedObject = object;
            break;
        }
    }
*/

    ////////////////////////////////////
    // Update properties after moving //
    ////////////////////////////////////

    bottom = GET_BOTTOM_COLLISION(player->collider);

    if (bottom && player->coyoteTimer != 0) {
        player->coyoteTimer = 0; // Reset the timer
    } else if (lastBottom && !bottom) {
        player->coyoteTimer = 1; // Start the timer if the player falls off the ground
    } else if (player->coyoteTimer > 0 && player->coyoteTimer <= MAX_COYOTE_TIME) { // Allow the timer to exceed the maximum by one frame
        player->coyoteTimer++;
    }

    bool inWater = HAS_TILE_COLLISION(player->collider->tileCollisionFlags, WATER_TILE_INDEX);

    if (lastInWater == true && inWater == false) {
        player->waterTimer = 0;
    }
    if (inWater) {
        player->waterTimer++;
        if (player->waterTimer > 240) {
            if ((player->waterTimer & 0x3F) == 0x3F) { // every 0b**********111111
                hud_updateHP(hp - 1);
            }

            if (player->waterTimer == MAX_U16) {
                player->waterTimer = 240;
            }
        }
    }

    // These values should be calculated here (after collision handling)
    player->isFalling = !bottom && player->velocity.y > FASTFIX32(0);
    if (player->isJumping && bottom) {
        player->isJumping = false;
    }

    // Update animations based on the player's state
    if (!bottom) {
        SPR_setAnim(player->sprite, ANIM_STAND);
    } else {
        if (player->velocity.x != FASTFIX32(0)) {

            // void playerFrameChanged(Sprite* sprite) {
            //     player->sprite->timer = 10;
            // }
            // SPR_setFrameChangeCallback(player->sprite, playerFrameChanged);

            SPR_setAnim(player->sprite, ANIM_WALK);
        } else {
            SPR_setAnim(player->sprite, ANIM_STAND);
        }
    }

    player->autoVelocity.x = FASTFIX32(0);
    player->autoVelocity.y = FASTFIX32(0);

    if (player->collider->tileCollisionFlags != 0) {
        environment_onTileCollidesWithPlayerInViewport();
    }
    if (collidedObject != NULL) {
        environment_onObjectCollidesWithPlayerInViewport(collidedObject);
    }
}

void player_updateSprite() {
    SPR_setPosition(player->sprite, player->collider->globalAABB.x.min - cameraPosition.x, player->collider->globalAABB.y.min + 1 - cameraPosition.y);
}