#include <genesis.h>
#include "engine.h"

#define PLAYER_WIDTH              24
#define PLAYER_HEIGHT             24
#define PLAYER_WIDTH_TILES        PLAYER_WIDTH / 8
#define PLAYER_HEIGHT_TILES       PLAYER_HEIGHT / 8

#define ANIM_STAND                0
#define ANIM_WALK                 1
#define ANIM_CLIMB                2

#define MAX_SPEED                 1 // скорость - кол-во пикселей которые прибавляются к позиции персонажа за фрейм

typedef struct {
    Sprite* sprite;       // Спрайт игрока
    AABB aabb;
    
    bool isJumping;       // Состояние прыжка
    bool isFalling;       // 
    bool isMoving;        // 

    Vect2D_s8 moving;     // Флажок движения. Имеет ненулевое значение по соответствующей координате если пользователь зажал кнопку движения). Используется для обновления ориентации спрайта и анимации
    Vect2D_ff32 velocity;   // Скорость движения. Используется для управления положением персонажа.
    Vect2D_s8 facingDirection;

    bool onGround;        // Находится ли персонаж на земле
    bool onCeiling;       // Находится ли персонаж в препятствие над его головой
    bool onLeftStuff;     // Упирается ли персонаж в препятствие слева от него
    bool onRightStuff;    // Упирается ли персонаж в препятствие справа от него

    s16 acceleration;

    u8 animIndex;

    s16 mapOverheight;
} Player;

Player* allocPlayer();
void player_init(Player* player, s16 startX, s16 startY, s16 mapOverheight);
void player_update(Player* player, u8* collisionsMap, u16 mapWTiles, u16 mapHTiles);
void player_setAnim(Player* player, u16 animIndex);
void player_move(Player* player, u8* collisionsMap, u16 mapWTiles, u16 mapHTiles);
bool player_checkMapScrollX(Player* player, s16 mapShiftX, u16 mapW);
bool player_checkMapScrollY(Player* player, s16 mapShiftY, u16 mapH);