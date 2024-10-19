#include <genesis.h>

typedef struct {
    u8 objType;
    Sprite *sprite; // Спрайт объекта
    AABB globalAABB;
    Vect2D_s16 screenPos;
    bool visible;
    u8 shift;
    bool dir;
} GameObject;

#define IS_CURR_OBJECT_VISIBLE (((currObject->screenPos.x + currObject->sprite->definition->w) >= 0 && (currObject->screenPos.x) < (SCREEN_WIDTH)) && ((currObject->screenPos.y + currObject->sprite->definition->h) >= 0 && (currObject->screenPos.y) < (SCREEN_HEIGHT)))

GameObject *allocGameObject();
void environment_init(u8* collisions);
void environment_update();
void environment_objectDelete();