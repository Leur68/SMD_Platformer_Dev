#pragma once
#include <genesis.h>

#define DEBUG                  1

#define SCREEN_WIDTH           320
#define SCREEN_HEIGHT          224
#define SCREEN_WIDTH_TILES     SCREEN_WIDTH  / 8  // 40
#define SCREEN_HEIGHT_TILES    SCREEN_HEIGHT / 8  // 28

#define SLOW_FADE              50
#define NORMAL_FADE            30
#define FAST_FADE              10

#define DIRECTION_LEFT         -1
#define DIRECTION_RIGHT        1
#define DIRECTION_UP           -1
#define DIRECTION_DOWN         1
#define DIRECTION_NONE         0

#define masPointer2(mas, i, j, jLen)  (*(mas + (i * jLen) + j))

typedef struct {
    ff32 min;
    ff32 max;
} AxisLineFF32;

typedef struct {
    s16 min;
    s16 max;
} AxisLineS16;

typedef struct {
    AxisLineFF32 x;
    AxisLineFF32 y;
    AxisLineS16 xTiles;
    AxisLineS16 yTiles;
} AABB;

typedef struct {
    s8 x;
    s8 y;
} Vect2D_s8;

void engine_init();
void engine_disableSoftReset(bool resetType);
void engine_setBlackCRAM();
void engine_setGreyCRAM();
void engine_fadeInImage(Image img, u16 numFrame);
void engine_fadeInPalette(const u16 * pal, u16 numFrame);
void engine_fadeInScreen(u16 numFrame);
void engine_fadeOutScreen(u16 numFrame);
void engine_drawInt(const char* text, s16 num, u16 x, u16 y);
bool engine_isTileSolid(u8* collisions, s16 xTile, s16 yTile, u16 mapWTiles, u16 mapHTiles);
AABB engine_checkMapArea(u8* collisions, AABB aabb, u16 mapWTiles, u16 mapHTiles);
bool engine_isOverlappingAxisLines(AxisLineFF32 x1, AxisLineFF32 x2);
bool engine_isOverlappingAABBs(AABB aabb1, AABB aabb2);
AABB engine_getTopAABB(AABB aabb);
AABB engine_getBottomAABB(AABB aabb);
AABB engine_getLeftAABB(AABB aabb);
AABB engine_getRightAABB(AABB aabb);
void engine_initAABBTileIndexes(AABB* aabb);
AABB engine_getNewAABB(AABB aabb, Vect2D_s8 velocity);
ff32 engine_roundUpByEight(ff32 x);