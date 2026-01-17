#include <global.h>

#define NONE_TILE_INDEX          0

#define SOLID_TILE_INDEX         1
#define TOP_SOLID_TILE_INDEX     8

#define BONUS_TILE_INDEX         2

#define M_X_PLATFORM_TILE_INDEX  3
#define M_Y_PLATFORM_TILE_INDEX  6
#define M_XY_PLATFORM_TILE_INDEX 7

#define M_X_PLATFORM_TOP_SOLID_TILE_INDEX  9
#define M_Y_PLATFORM_TOP_SOLID_TILE_INDEX  10
#define M_XY_PLATFORM_TOP_SOLID_TILE_INDEX 11

#define STAIRS_TILE_INDEX        4
#define WATER_TILE_INDEX         5

#define IS_MOVING_PLATFORM(tileIndex)            (IS_MOVING_SOLID_PLATFORM(tileIndex) || IS_MOVING_TOP_SOLID_PLATFORM(tileIndex))
#define IS_MOVING_SOLID_PLATFORM(tileIndex)      ((tileIndex) == M_X_PLATFORM_TILE_INDEX || (tileIndex) == M_Y_PLATFORM_TILE_INDEX || (tileIndex) == M_XY_PLATFORM_TILE_INDEX)
#define IS_MOVING_TOP_SOLID_PLATFORM(tileIndex)  ((tileIndex) == M_X_PLATFORM_TOP_SOLID_TILE_INDEX || (tileIndex) == M_Y_PLATFORM_TOP_SOLID_TILE_INDEX || (tileIndex) == M_XY_PLATFORM_TOP_SOLID_TILE_INDEX)
#define IS_OBJECT(tileIndex)                     ((tileIndex) == BONUS_TILE_INDEX || IS_MOVING_PLATFORM(tileIndex))
#define IS_SOLID_OBJECT(tileIndex)               (IS_MOVING_SOLID_PLATFORM(tileIndex))
#define IS_TOP_SOLID_OBJECT(tileIndex)           (IS_MOVING_TOP_SOLID_PLATFORM(tileIndex))

typedef struct {
    u8 tileIndex; // для определения типа объекта
    Sprite *sprite;
    AABB globalAABB;
    bool visible;
    u8 timer;
    Vect2D_s16 moving;
} GameObject;

GameObject *allocGameObject();
void environment_init();
void environment_updateObjects();
void environment_freeObject(GameObject *object);
void environment_updateSprites();
void environment_cleanup();