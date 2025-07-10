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
    player->collider = (Collider *)MEM_alloc(sizeof(Collider));
    aabb_set(&player->collider->globalAABB, (Vect2D_u16){startX, startY + mapMaxCameraPosY}); // Position
    player->posBuffer = (Vect2D_ff32){FASTFIX32(player->collider->globalAABB.x.min), FASTFIX32(player->collider->globalAABB.y.min)};
    player->collider->facingDirection = DIRECTION_NONE;
    player->collider->groundCollisionData = 0;
    player->collider->tileCollisionFlags = 0;

    // Movement
    player->velocity = (Vect2D_ff32){FASTFIX32(0), FASTFIX32(0)};
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
    // Update properties before moving

    u8 bottom = GET_BOTTOM_COLLISION(player->collider);

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

    bool onStairs = HAS_TILE_COLLISION(player->collider, STAIRS_TILE_INDEX);
    if (player->isClimbing && !onStairs) {
        player->isClimbing = false;
    }

// Add gravity
#if (!DEBUG_FREE_MOVE_MODE)
    // в воздухе, но не на лестнице
    // в прыжке
    if ((!bottom && !HAS_TILE_COLLISION(player->collider, STAIRS_TILE_INDEX)) || player->isJumping) {
        if (player->velocity.y < FASTFIX32(GRAVITY)) {
            player->velocity.y += FASTFIX32(GRAVITY_ACCELERATION);
        }
        if (player->velocity.y > FASTFIX32(GRAVITY)) {
            player->velocity.y = FASTFIX32(GRAVITY);
        }
    }
#endif

    // Update facing direction
    player->collider->facingDirection = DIRECTION_NONE;

    if (player->velocity.x < FASTFIX32(0)) {

        player->collider->facingDirection |= DIRECTION_LEFT;
        SPR_setHFlip(player->sprite, false); // Update sprite's horizontal orientation

    } else if (player->velocity.x > FASTFIX32(0)) {

        player->collider->facingDirection |= DIRECTION_RIGHT;
        SPR_setHFlip(player->sprite, true); // Update sprite's horizontal orientation

    }
    if (player->velocity.y < FASTFIX32(0)) {

        player->collider->facingDirection |= DIRECTION_UP;

    } else if (player->velocity.y >= FASTFIX32(0)) { // falling or standing on ground

        player->collider->facingDirection |= DIRECTION_DOWN;

    }
    
#if (DEBUG_INTERRUPT)
    if (player->collider->facingDirection > 10 || player->collider->facingDirection == 3 || player->collider->facingDirection == 7) {
        char str[3];
        intToStr(player->collider->facingDirection, str, 3);
        SYS_die("The value of player.collider.facingDirection", "is not valid.", "", str, NULL);
    }
#endif

    u8 lastLower = bottom;
    bool lastInWater = HAS_TILE_COLLISION(player->collider, WATER_TILE_INDEX);

    // Handle collisions and move the player
    player_move();

    // Update properties after moving

    bottom = GET_BOTTOM_COLLISION(player->collider);

    if (bottom && player->coyoteTimer != 0) {
        player->coyoteTimer = 0; // Reset the timer
    } else if (lastLower && !bottom) {
        player->coyoteTimer = 1; // Start the timer if the player falls off the ground
    } else if (player->coyoteTimer > 0 && player->coyoteTimer <= MAX_COYOTE_TIME) { // Allow the timer to exceed the maximum by one frame
        player->coyoteTimer++;
    }

    bool inWater = HAS_TILE_COLLISION(player->collider, WATER_TILE_INDEX);

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

    if (player->collider->tileCollisionFlags != 0) {
        environment_onTileCollidesWithPlayerInViewport();
    }
}

void player_move() {

    // Do not check collisions if the player is stationary

    if (player->velocity.x != FASTFIX32(0) || player->velocity.y != FASTFIX32(0) || player->autoVelocity.x != FASTFIX32(0) || player->autoVelocity.y != FASTFIX32(0)) {

        // Calculate the number of pixels the player will move during the current frame
        player_calculateSubpixelMovement();

        // Update the player's coordinates
        aabb_shift(&player->collider->globalAABB, player->movedPixels);

    } else {
        player->movedPixels.x = 0;
        player->movedPixels.y = 0;
    }

    // Handle collisions

    player_handleCollisions();
}

void player_calculateSubpixelMovement() {
    // Move the player using subpixel precision
    // On each iteration, add the velocity to the "buffer" position of the player.
    // Then check: if the integer part of the buffer changes, add the number of pixels by which the buffer position changed to the actual position.

    player->posBuffer.x += (player->velocity.x + player->autoVelocity.x);
    player->posBuffer.y += (player->velocity.y + player->autoVelocity.y);

#if (DEBUG_INTERRUPT)
    if (player->posBuffer.x < FASTFIX32(0)) {
        SYS_die("in player_calculateSubpixelMovement", "posBuffer.x is negative", NULL);
    }
    if (player->posBuffer.y < FASTFIX32(0)) {
        SYS_die("in player_calculateSubpixelMovement", "posBuffer.y is negative", NULL);
    }
#endif

    s16 newPosX = FF32_toInt(FF32_int(player->posBuffer.x));
    s16 newPosY = FF32_toInt(FF32_int(player->posBuffer.y));

    player->movedPixels.x = newPosX - player->collider->globalAABB.x.min;
    player->movedPixels.y = newPosY - player->collider->globalAABB.y.min;
}

void player_handleCollisions() {

    collision_check(player->collider);

#if (!DEBUG_FREE_MOVE_MODE)
    if (HAS_ANY_COLLISION(player->collider)) {
        // Check if the player is stuck in an obstacle
        // Calculate the amount of displacement along both axes
        Vect2D_s16 shift = {0, 0};

        u8 left     = GET_LEFT_COLLISION(player->collider);
        u8 right    = GET_RIGHT_COLLISION(player->collider);
        u8 top      = GET_TOP_COLLISION(player->collider);
        u8 bottom   = GET_BOTTOM_COLLISION(player->collider);
        bool ground = HAS_GROUND_COLLISION(player->collider);

        if (left > 1) {

            shift.x = left - 1;

        } else if (right > 1) {

            shift.x = (1 - right);

        }
        if (top > 1) {

            shift.y = top - 1;

        } else if (bottom > 1) {

            shift.y = (1 - bottom);

        }
        // Push the player out of the obstacle if stuck (this happens only at speeds greater than 1)
        if (shift.x != 0 || shift.y != 0) {
            // Push out
            aabb_shift(&player->collider->globalAABB, shift);

            // Adjust variables
            player->posBuffer.x += FASTFIX32(shift.x);
            player->posBuffer.y += FASTFIX32(shift.y);
            player->movedPixels.x += shift.x;
            player->movedPixels.y += shift.y;

            // Recalculate collisions

            collision_check(player->collider);

            left   = GET_LEFT_COLLISION(player->collider);
            right  = GET_RIGHT_COLLISION(player->collider);
            top    = GET_TOP_COLLISION(player->collider);
            bottom = GET_BOTTOM_COLLISION(player->collider);
            ground = HAS_GROUND_COLLISION(player->collider);
        }

        // Stop velocity on the corresponding axis when colliding with an obstacle
        if (left) {
            if (player->velocity.x < FASTFIX32(0)) {
                player->velocity.x = FASTFIX32(0);
            }
            if (player->autoVelocity.x < FASTFIX32(0)) {
                player->autoVelocity.x = FASTFIX32(0);
            }
        } else if (right) {
            if (player->velocity.x > FASTFIX32(0)) {
                player->velocity.x = FASTFIX32(0);
            }
            if (player->autoVelocity.x > FASTFIX32(0)) {
                player->autoVelocity.x = FASTFIX32(0);
            }
        }
        if (top) {
            if (player->velocity.y < FASTFIX32(0)) {
                player->velocity.y = FASTFIX32(0);
            }
            if (player->autoVelocity.y < FASTFIX32(0)) {
                player->autoVelocity.y = FASTFIX32(0);
            }
        } else if (bottom) {
            if (player->velocity.y > FASTFIX32(0)) {
                player->velocity.y = FASTFIX32(0);
            }
            if (player->autoVelocity.y > FASTFIX32(0)) {
                player->autoVelocity.y = FASTFIX32(0);
            }
        }
    }
#endif
}

void player_updateSprite() {
    SPR_setPosition(player->sprite, player->collider->globalAABB.x.min - cameraPosition.x, player->collider->globalAABB.y.min - cameraPosition.y);
}