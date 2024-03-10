#pragma once
#include <genesis.h>

#define DEBUG_PLAYER           0
#define DEBUG_COLLISIONS       0
#define DEBUG_GAME             1

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

#define COL_NONE               0
#define COL_SHIFT_L            1
#define COL_SHIFT_R            2
#define COL_SHIFT_T            3
#define COL_SHIFT_B            4
#define COL_SHIFT_LT           5
#define COL_SHIFT_LB           6
#define COL_SHIFT_RT           7
#define COL_SHIFT_RB           8
#define COL_SHIFT_LR           9
#define COL_SHIFT_TB           10

#if (DEBUG_COLLISIONS)
    extern Sprite* tileCursorsR[12];
    extern Sprite* playerCursor;
#endif

#define masPointer2(mas, i, j, jLen)  (*(mas + (i * jLen) + j))

typedef struct {
    ff32 min;
    ff32 max;
} AxisLine_ff32;

typedef struct {
    ff32 x;
    ff32 y;
} Vect2D_ff32;

typedef struct {
    s16 min;
    s16 max;
} AxisLine_s16;

typedef struct {
    AxisLine_s16 x;
    AxisLine_s16 y;
    AxisLine_s16 xTiles;
    AxisLine_s16 yTiles;
    bool exists;
} AABB;

typedef struct {
    s8 x;
    s8 y;
} Vect2D_s8;

#if (DEBUG_GAME)
    extern bool isDebug;
#endif
extern bool paused;
extern bool xyzButtons;

void engine_init();
void engine_disableSoftReset(bool resetType);
void engine_setBlackCRAM();
void engine_setGreyCRAM();
void engine_fadeInImage(Image img, u16 numFrame);
void engine_fadeInPalette(const u16 * pal, u16 numFrame);
void engine_fadeInScreen(u16 numFrame);
void engine_fadeOutScreen(u16 numFrame);
void engine_drawDebugInt(const char* text, s16 num, u16 x, u16 y);
void engine_drawInt(s16 num, u16 x, u16 y, u16 len);
void engine_drawFix32(const char* text, f32 num, u16 x, u16 y);

bool engine_isTileSolid(u8* collisions, s16 xTile, s16 yTile, u16 mapWTiles, u16 mapHTiles);
AABB engine_checkMapArea(u8* collisions, AABB aabb, u16 mapWTiles, u16 mapHTiles);
bool engine_isOverlappingAxisLines(AxisLine_s16 x1, AxisLine_s16 x2);
bool engine_isOverlappingAABBs(AABB aabb1, AABB aabb2);
AABB engine_getTopAABB(AABB aabb);
AABB engine_getBottomAABB(AABB aabb);
AABB engine_getLeftAABB(AABB aabb);
AABB engine_getRightAABB(AABB aabb);
void engine_initAABBTileIndexes(AABB* aabb);
void engine_shiftAABB(AABB* aabb, Vect2D_s16 shift);
void engine_setAABB(AABB* aabb, s16 x, s16 y);
s16 engine_roundUpByEight(s16 x);
s16 engine_roundDownByEight(s16 x);
bool engine_isMultipleOfEight(int num);
void engine_checkCollisions(AABB aabb, u8* collisionsMap, u16 mapWTiles, u16 mapHTiles, Vect2D_s8 direction, u8* left, u8* right, u8* top, u8* bottom);
u8 engine_getIntersectionLen(AxisLine_s16 a, AxisLine_s16 b);

void engine_showFPS(u16 asFloat, u16 x, u16 y);