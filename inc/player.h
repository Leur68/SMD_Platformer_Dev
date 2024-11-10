#include <global.h>

#define PLAYER_WIDTH              24
#define PLAYER_HEIGHT             24
#define PLAYER_WIDTH_TILES        PLAYER_WIDTH / 8
#define PLAYER_HEIGHT_TILES       PLAYER_HEIGHT / 8

#define ANIM_STAND                0
#define ANIM_WALK                 1
#define ANIM_CLIMB                2

#define MAX_COYOTE_TIME           10 // максимальное кол-во фреймов после покидания земли, когда еще можно активировать прыжок
#define MAX_JUMP_TIME             30 // максимальное кол-во фреймов перед преземлением, когда уже можно активировать прыжок

typedef struct {
    Sprite *sprite; // Спрайт игрока
    AABB globalAABB;
    Vect2D_u16 screenPos;
    Vect2D_ff32 posBuffer; // Для расчета скорости
    Vect2D_ff32 velocity;  // Скорость движения. Используется для управления положением персонажа.
    Vect2D_ff32 autoVelocity;  // Скорость движения. Используется для управления положением персонажа.
    u8 facingDirection;
    Vect2D_s16 movedPixels;

    bool isJumping;
    bool isFalling;
    bool isMoving;
    bool isAutoMoving;

    u8 inLeftObstacle;
    u8 inRightObstacle;
    u8 inUpperObstacle;
    u8 inLowerObstacle;

    u8 coyoteTimer;
    u8 jumpTimer;
} Player;

#define hasPlayerMovedByX (player->movedPixels.x != 0)
#define hasPlayerMovedByY (player->movedPixels.y != 0)
#define hasPlayerMoved (hasPlayerMovedByX || hasPlayerMovedByY)

Player *allocPlayer();
void player_init(u16 startX, u16 startY);
void player_update();
void player_move();
void player_handleCollisions();
void player_calculateVelocity();