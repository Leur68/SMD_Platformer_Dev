#include <global.h>

#define NONE_TILE_INDEX        0
#define SOLID_TILE_INDEX       1
#define BONUS_TILE_INDEX       2
#define M_PLATFORM_TILE_INDEX  3
#define STAIRS_TILE_INDEX      4

typedef struct {
    u8 objType;
    Sprite *sprite;
    AABB globalAABB;
    bool visible;
    u8 shift;
    u8 facingDirection;
} GameObject;

#define isCurrObjectVisible(screenX, screenY) (((screenX + currObject->sprite->definition->w) >= 0 && (screenX) < (SCREEN_WIDTH)) && ((screenY + currObject->sprite->definition->h) >= 0 && (screenY) < (SCREEN_HEIGHT)))
#define hasCurrObjectCollidesWithPlayer aabb_intersects(player->globalAABB, currObject->globalAABB)

GameObject *allocGameObject();
void environment_init(u8 *collisions);
void environment_updateObjects();
void environment_objectDelete();
void environment_updateSprites();