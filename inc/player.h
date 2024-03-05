#include <genesis.h>
#include "engine.h"

#define PLAYER_WIDTH              24
#define PLAYER_HEIGHT             24
#define PLAYER_WIDTH_TILES        PLAYER_WIDTH / 8
#define PLAYER_HEIGHT_TILES       PLAYER_HEIGHT / 8

#define ANIM_STAND                0
#define ANIM_WALK                 1
#define ANIM_CLIMB                2

typedef void MapScrollFunc(s16 scrollX, s16 scrollY);

typedef struct {
    Sprite *sprite; // Спрайт игрока
    AABB aabb;
    Vect2D_ff32 posBuffer; // Для расчета скорости
    Vect2D_ff32 velocity;  // Скорость движения. Используется для управления положением персонажа.
    Vect2D_s8 facingDirection;

    bool isJumping;
    bool isFalling;
    bool isMoving;

    u8 inLeftObstacle;
    u8 inRightObstacle;
    u8 inUpperObstacle;
    u8 inLowerObstacle;

    MapScrollFunc *mapScrollFunc;
} Player;

Player *allocPlayer();
void player_init(Player *player, s16 startX, s16 startY, s16 mapOverheight, MapScrollFunc *mapScrollFunc);
void player_update(Player *player, u8 *collisionsMap, u16 mapWTiles, u16 mapHTiles);
void player_move(Player *player, u8 *collisionsMap, u16 mapWTiles, u16 mapHTiles);
bool player_isScrollMapX(Player *player, u16 mapW, s16 movedPixelsX);
bool player_isScrollMapY(Player *Player, u16 mapH, s16 movedPixelsY);