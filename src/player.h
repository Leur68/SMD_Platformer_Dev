#include <global.h>

#define PLAYER_WIDTH              24  // Player width in pixels
#define PLAYER_HEIGHT             24  // Player height in pixels
#define PLAYER_WIDTH_TILES        PLAYER_WIDTH / 8  // Player width in tiles
#define PLAYER_HEIGHT_TILES       PLAYER_HEIGHT / 8 // Player height in tiles

#define ANIM_STAND                0  // Animation ID for standing
#define ANIM_WALK                 1  // Animation ID for walking
#define ANIM_CLIMB                2  // Animation ID for climbing

#define MAX_COYOTE_TIME           10 // Maximum number of frames after leaving the ground during which the player can still jump
#define MAX_JUMP_TIME             30 // Maximum number of frames before landing during which the player can prepare to jump

typedef struct {
    Sprite *sprite;           // Player's sprite

    Collider *collider;
    Vect2D_ff32 posBuffer;    // Position buffer used for movement calculations with subpixel precision

    Vect2D_ff32 velocity;     // Movement velocity, used to control the player's position
    Vect2D_ff32 autoVelocity; // Automated movement velocity, used for specific behaviors or scripts
    Vect2D_s16 movedPixels;   // Number of pixels the player has moved in the current frame

    bool isJumping;    // Indicates if the player is jumping
    bool isFalling;    // Indicates if the player is falling
    bool isClimbing;   // Indicates if the player is climbing
    bool decelerating; // Indicates if the player is decelerating (slowing down)

    u8 coyoteTimer; // Timer for the "coyote time" mechanic (frames left to jump after leaving the ground)
    u8 jumpTimer;   // Timer for the jump mechanic (tracks frames spent in the air during a jump)
    u16 waterTimer;
} Player;

#define hasPlayerMovedByX (player->movedPixels.x != 0)          // Checks if the player has moved along the X-axis
#define hasPlayerMovedByY (player->movedPixels.y != 0)          // Checks if the player has moved along the Y-axis
#define hasPlayerMoved (hasPlayerMovedByX || hasPlayerMovedByY) // Checks if the player has moved in any direction

/**
 * Allocates memory for a new Player object.
 * @return A pointer to the newly allocated Player.
 */
Player *allocPlayer();

/**
 * Initializes the player at the given starting position.
 * @param startX The starting X coordinate in pixels.
 * @param startY The starting Y coordinate in pixels.
 */
void player_init(u16 startX, u16 startY);

/**
 * Updates the player's state.
 */
void player_update();

/**
 * Updates the player's sprite position on the screen.
 */
void player_updateSprite();