#include <genesis.h>
#include "engine.h"

#define NPC_WIDTH              24
#define NPC_HEIGHT             24
#define NPC_WIDTH_TILES        PLAYER_WIDTH / 8
#define NPC_HEIGHT_TILES       PLAYER_HEIGHT / 8
#define ANIM_STAND                0
#define ANIM_WALK                 1
#define ANIM_CLIMB                2

typedef struct {
    Sprite* sprite;     // Спрайт игрока
    AABB aabb;
    Vect2D_s8 direction;

    bool isJumping;     // Состояние прыжка
    bool isClimbing;    // Состояние взбирания на лестницу
    bool isMoving;      // Состояние движения

    bool inGround;      // Находится ли персонаж на земле
    bool inCeiling;     // Находится ли персонаж в препятствие над его головой
    bool onLeftStuff;   // Упирается ли персонаж в препятствие слева от него
    bool onRightStuff;  // Упирается ли персонаж в препятствие справа от него

    u8 animIndex;
} NPC;

NPC* allocNPC();
void npc_init(NPC* npc, s16 startX, s16 startY);
void npc_update(NPC* npc, u8* collisionsMap, u16 mapW, u16 mapH);
void npc_setAnim(NPC* npc, u16 animIndex);
void npc_checkCollision(NPC* npc, u8* collisionsMap, u16 mapW, u16 mapH);