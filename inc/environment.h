#include <global.h>

#define NONE_TILE_INDEX          0
#define SOLID_TILE_INDEX         1
#define BONUS_TILE_INDEX         2
#define M_X_PLATFORM_TILE_INDEX  3
#define STAIRS_TILE_INDEX        4
#define WATER_TILE_INDEX         5
#define M_Y_PLATFORM_TILE_INDEX  6

typedef struct {
    u8 objType;
    Sprite *sprite;
    AABB globalAABB;
    bool visible;
    u8 shift;
    u8 facingDirection;
} GameObject;

GameObject *allocGameObject();
void environment_init();
void environment_updateObjects();
void environment_freeObject();
void environment_updateSprites();
void environment_cleanup();