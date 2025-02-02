#include "../inc/global.h"

Player *allocPlayer() {
    return (Player *)MEM_alloc(sizeof(Player));
}

void player_init(u16 startX, u16 startY) {
    // Sprite
    player->sprite = SPR_addSpriteSafe(&player_sprite, startX, startY, TILE_ATTR(PLAYER_PALETTE, 0, false, true));
    SPR_setAlwaysOnTop(player->sprite);
    PAL_setPalette(PLAYER_PALETTE, player_sprite.palette->data, DMA);

    // Position
    aabb_set(&player->globalAABB, (Vect2D_u16){startX, startY + mapMaxCameraPosY});
    player->posBuffer = (Vect2D_ff32){intToFastFix32(player->globalAABB.x.min), intToFastFix32(player->globalAABB.y.min)};

    // Movement
    player->velocity = (Vect2D_ff32){FASTFIX32(0), FASTFIX32(0)};
    player->autoVelocity = (Vect2D_ff32){FASTFIX32(0), FASTFIX32(0)};
    player->movedPixels = (Vect2D_s16){0, 0};

    player->facingDirection = DIRECTION_NONE;
    player->coyoteTimer = 0;
    player->isJumping = false;
    player->isFalling = false;
    player->decelerating = false;

    // Collisions
    player->inUpperObstacle = false;
    player->inLowerObstacle = false;
    player->inLeftObstacle = false;
    player->inRightObstacle = false;
}

void player_update() {
    // Update properties before moving

    // Handle deceleration when movement keys are released
    if (player->decelerating) {
        if (player->inLowerObstacle) {
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

// Add gravity
#if (!DEBUG_FREE_MOVE_MODE)
    if (!player->inLowerObstacle) {
        if (player->velocity.y < FASTFIX32(GRAVITY)) {
            player->velocity.y += FASTFIX32(GRAVITY_ACCELERATION);
        }
        if (player->velocity.y > FASTFIX32(GRAVITY)) {
            player->velocity.y = FASTFIX32(GRAVITY);
        }
    }
#endif

    // Update facing direction
    player->facingDirection = DIRECTION_NONE;

    if (player->velocity.x < FASTFIX32(0)) {

        player->facingDirection |= DIRECTION_LEFT;
        SPR_setHFlip(player->sprite, false); // Update sprite's horizontal orientation

    } else if (player->velocity.x > FASTFIX32(0)) {

        player->facingDirection |= DIRECTION_RIGHT;
        SPR_setHFlip(player->sprite, true); // Update sprite's horizontal orientation

    }
    if (player->velocity.y < FASTFIX32(0)) {

        player->facingDirection |= DIRECTION_UP;

    } else if (player->velocity.y >= FASTFIX32(0)) { // falling or standing on ground

        player->facingDirection |= DIRECTION_DOWN;

    }
    
#if (DEBUG_INTERRUPT)
    if (player->facingDirection > 10 || player->facingDirection == 3 || player->facingDirection == 7) {
        char str[3];
        intToStr(player->facingDirection, str, 3);
        SYS_die("The value of player.facingDirection", "is not valid.", "", str, NULL);
    }
#endif

    u8 lastLower = player->inLowerObstacle;

    // Handle collisions and move the player
    player_move();

    // Update properties after moving

    if (player->inLowerObstacle && player->coyoteTimer != 0) {
        player->coyoteTimer = 0; // Reset the timer
    } else if (lastLower && !player->inLowerObstacle) {
        player->coyoteTimer = 1; // Start the timer if the player falls off the ground
    } else if (player->coyoteTimer > 0 && player->coyoteTimer <= MAX_COYOTE_TIME) { // Allow the timer to exceed the maximum by one frame
        player->coyoteTimer++;
    }

    // These values should be calculated here (after collision handling)
    player->isJumping = !player->inLowerObstacle && player->velocity.y < FASTFIX32(0);
    player->isFalling = !player->inLowerObstacle && player->velocity.y > FASTFIX32(0);

    // Update animations based on the player's state
    if (!player->inLowerObstacle) {
        SPR_setAnim(player->sprite, ANIM_STAND);
    } else if (player->inLowerObstacle) {

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
}

void player_move() {
    // Do not check collisions if the player is stationary

    if (player->velocity.x != FASTFIX32(0) || player->velocity.y != FASTFIX32(0) || player->autoVelocity.x != FASTFIX32(0) || player->autoVelocity.y != FASTFIX32(0)) {

        // Calculate the number of pixels the player will move during the current frame
        player_calculateSubpixelMovement();

        // Update the player's coordinates
        aabb_shift(&player->globalAABB, player->movedPixels);

    } else {
        player->movedPixels.x = 0;
        player->movedPixels.y = 0;
    }

    // Handle collisions

    environment_updateObjects(); // Update objects for subsequent proper collision calculations with the character

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

    s16 newPosX = fastFix32ToInt(fastFix32Int(player->posBuffer.x));
    s16 newPosY = fastFix32ToInt(fastFix32Int(player->posBuffer.y));

    player->movedPixels.x = newPosX - player->globalAABB.x.min;
    player->movedPixels.y = newPosY - player->globalAABB.y.min;
}

void player_handleCollisions() {
    player->autoVelocity.x = FASTFIX32(0);

    collision_check(player->globalAABB, player->facingDirection,
                    &player->inLeftObstacle,
                    &player->inRightObstacle,
                    &player->inUpperObstacle,
                    &player->inLowerObstacle,
                    &player->groundCollision);

#if (!DEBUG_FREE_MOVE_MODE)
    bool overlapped = (player->inLeftObstacle + player->inRightObstacle + player->inUpperObstacle + player->inLowerObstacle) != 0;
    if (overlapped) {
        // Check if the player is stuck in an obstacle
        // Calculate the amount of displacement along both axes
        Vect2D_s16 shift = {0, 0};

        if (player->inLeftObstacle > 1) {
            shift.x = player->inLeftObstacle - 1;
        } else if (player->inRightObstacle > 1) {
            shift.x = (1 - player->inRightObstacle);
        }
        if (player->inUpperObstacle > 1) {
            shift.y = player->inUpperObstacle - 1;
        } else if (player->inLowerObstacle > 1) {
            shift.y = (1 - player->inLowerObstacle);
        }
        // Push the player out of the obstacle if stuck (this happens only at speeds greater than 1)
        if (shift.x != 0 || shift.y != 0) {
            // Push out
            aabb_shift(&player->globalAABB, shift);

            // Adjust variables
            player->posBuffer.x += FASTFIX32(shift.x);
            player->posBuffer.y += FASTFIX32(shift.y);
            player->movedPixels.x += shift.x;
            player->movedPixels.y += shift.y;

            // Recalculate collisions

            collision_check(player->globalAABB, player->facingDirection,
                            &player->inLeftObstacle,
                            &player->inRightObstacle,
                            &player->inUpperObstacle,
                            &player->inLowerObstacle,
                            &player->groundCollision);
        }

        // Stop velocity on the corresponding axis when colliding with an obstacle
        if (player->inLeftObstacle) {
            if (player->velocity.x < FASTFIX32(0)) {
                player->velocity.x = FASTFIX32(0);
            }
            if (player->autoVelocity.x < FASTFIX32(0)) {
                player->autoVelocity.x = FASTFIX32(0);
            }
        } else if (player->inRightObstacle) {
            if (player->velocity.x > FASTFIX32(0)) {
                player->velocity.x = FASTFIX32(0);
            }
            if (player->autoVelocity.x > FASTFIX32(0)) {
                player->autoVelocity.x = FASTFIX32(0);
            }
        }
        if (player->inUpperObstacle) {
            if (player->velocity.y < FASTFIX32(0)) {
                player->velocity.y = FASTFIX32(0);
            }
            if (player->autoVelocity.y < FASTFIX32(0)) {
                player->autoVelocity.y = FASTFIX32(0);
            }
        } else if (player->inLowerObstacle) {
            if (player->velocity.y > FASTFIX32(0)) {
                player->velocity.y = FASTFIX32(0);
            }
            if (player->autoVelocity.y > FASTFIX32(0)) {
                player->autoVelocity.y = FASTFIX32(0);
            }
        }
        
        if (collidedObject != NULL && player->inLowerObstacle && !player->groundCollision) {
            switch (collidedObject->facingDirection) {
            case DIRECTION_RIGHT:
                player->autoVelocity.x = FASTFIX32(1.0);
                break;
            case DIRECTION_LEFT:
                player->autoVelocity.x = FASTFIX32(-1.0);
            }
        }
    }
#endif
}

void player_updateSprite() {
    SPR_setPosition(player->sprite, player->globalAABB.x.min - cameraPosition.x, player->globalAABB.y.min - cameraPosition.y);
}