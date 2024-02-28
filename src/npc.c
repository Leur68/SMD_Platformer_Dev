#include "../inc/game.h"

NPC* allocNPC() {
    return (NPC*) MEM_alloc(sizeof(NPC));
}

void npc_init(NPC* npc, s16 startX, s16 startY) {
    // Спрайт
    npc->sprite = SPR_addSprite(&player_sprite, startX, startY, TILE_ATTR(PLAYER_PALETTE, 0, false, true));
    PAL_setPalette(PLAYER_PALETTE, player_sprite.palette->data, DMA);
    // Положение и перемещение
    npc->aabb.x.min = startX;
    npc->aabb.y.min = startY;
    npc->aabb.x.max = npc->aabb.x.min + NPC_WIDTH - 1;
    npc->aabb.y.max = npc->aabb.y.min + NPC_WIDTH - 1;
    npc->aabb.xTiles.min = npc->aabb.x.min >> 3;
    npc->aabb.xTiles.max = npc->aabb.x.max >> 3;
    npc->aabb.yTiles.min = npc->aabb.y.min >> 3;
    npc->aabb.yTiles.max = npc->aabb.y.max >> 3;
    npc->direction.x = DIRECTION_NONE;
    npc->direction.y = DIRECTION_NONE;
    // Состояние
    npc->isJumping = false;
    npc->isClimbing = false;
    npc->isMoving = false;
    npc->inGround = false;
}

void npc_update(NPC* npc, u8* collisionsMap, u16 mapW, u16 mapH) {
    // Обновляем пространственную ориентацию спрайта
    if (npc->direction.x == DIRECTION_RIGHT) {
        SPR_setHFlip(npc->sprite, false);
    }
    if (npc->direction.x == DIRECTION_LEFT) {
        SPR_setHFlip(npc->sprite, false);
    }

    // Добавляем гравитацию
    if (npc->direction.y == DIRECTION_NONE) {
        npc->direction.y = DIRECTION_DOWN;
    }

    // Обрабатываем коллизии
    npc_checkCollision(npc, collisionsMap, mapW, mapH);
    
    // Обновляем позицию игрока в пространстве
    SPR_setPosition(npc->sprite, npc->aabb.x.min, npc->aabb.y.min);

    // Обновляем анимации на основе состояния персонажа
    if (npc->isClimbing) {
        npc_setAnim(npc, ANIM_CLIMB);
    } else if (npc->isMoving) {
        npc_setAnim(npc, ANIM_WALK);
    } else {
        npc_setAnim(npc, ANIM_STAND);
    }
}

// Обновление анимации персонажа
void npc_setAnim(NPC* npc, u16 animIndex) {
    if (npc->animIndex != animIndex) {
        // Меняем и обнуляем анимацию только если она изменилась - если не изменилась, то она будет выполняться циклически
        npc->animIndex = animIndex;
    }
    SPR_setAnim(npc->sprite, npc->animIndex);
}

void npc_checkCollision(NPC* npc, u8* collisionsMap, u16 mapW, u16 mapH) {
    // Находим AABB персонажа, для которого нужно проверить коллизии и принять решение о дальнейшем движении персонажа
    

}
