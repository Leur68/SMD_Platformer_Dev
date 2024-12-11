#include <global.h>

#define NONE_TILE_INDEX        0
#define SOLID_TILE_INDEX       1
#define BONUS_TILE_INDEX       2
#define M_PLATFORM_TILE_INDEX  3 // оставляем пропуски для определения смещения на 8 пикселей в любую сторону по горизонтали
#define STAIRS_TILE_INDEX      4

typedef struct {
    u8 objType;
    Sprite *sprite; // Спрайт объекта
    AABB globalAABB;
    Vect2D_s16 screenPos;
    bool visible;
    u8 shift;
    bool dir;
} GameObject;

#define isCurrObjectVisible (((currObject->screenPos.x + currObject->sprite->definition->w) >= 0 && (currObject->screenPos.x) < (SCREEN_WIDTH)) && ((currObject->screenPos.y + currObject->sprite->definition->h) >= 0 && (currObject->screenPos.y) < (SCREEN_HEIGHT)))
#define hasCurrObjectCollidesWithPlayer aabb_intersects(player->globalAABB, currObject->globalAABB)

GameObject *allocGameObject();
void environment_init(u8* collisions);
void environment_update();
void environment_objectDelete();