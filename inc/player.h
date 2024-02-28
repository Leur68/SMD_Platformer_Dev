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
    Sprite* sprite;         // Спрайт игрока
    AABB aabb;
    Vect2D_ff32 posBuffer;  // Для расчета скорости
    Vect2D_ff32 velocity;   // Скорость движения. Используется для управления положением персонажа.
    Vect2D_s8 moving;       // Флажок движения. Имеет ненулевое значение по соответствующей координате если пользователь зажал кнопку движения). Используется для обновления ориентации спрайта и анимации
    u8 animIndex;
    Vect2D_s8 facingDirection;

    bool isJumping;
    bool isFalling;
    bool isMoving;

    u8 inGround;
    u8 inCeiling;
    u8 inLeftObstacle;
    u8 inRightObstacle;
    
    MapScrollFunc* mapScrollFunc;
} Player;

Player* allocPlayer();
void player_init(Player* player, s16 startX, s16 startY, s16 mapOverheight, MapScrollFunc* mapScrollFunc);
void player_update(Player* player, u8* collisionsMap, u16 mapWTiles, u16 mapHTiles);
void player_setAnim(Player* player, u16 animIndex);
void player_move(Player* player, u8* collisionsMap, u16 mapWTiles, u16 mapHTiles);
bool player_checkMapScrollX(Player* player, u16 mapW);
bool player_checkMapScrollY(Player* Player, u16 mapH);