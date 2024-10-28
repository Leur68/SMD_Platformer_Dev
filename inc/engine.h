#pragma once
#include <genesis.h>

#define DEBUG                  0 // Боковая панель (WINDOW PLANE) с отладочной информацией
#define DEBUG_COLLISIONS       0 // Отключение гравитации, свободное перемещение. Визуальная демонстрация тайлов, пересекающихся с персонажем
#define DEBUG_GAME             1 // Доступность режима дебага (замедления). При нажатии на mode игра сильно замедляется

#define SCREEN_WIDTH           320
#define SCREEN_HEIGHT          224
#define SCREEN_WIDTH_TILES     (SCREEN_WIDTH  / 8)  // 40
#define SCREEN_HEIGHT_TILES    (SCREEN_HEIGHT / 8)  // 28

#define SLOW_FADE              50
#define NORMAL_FADE            30
#define FAST_FADE              10

#define DIRECTION_NONE        0b00000000  // 0
#define DIRECTION_LEFT        0b00000001  // 1
#define DIRECTION_RIGHT       0b00000010  // 2
#define DIRECTION_UP          0b00000100  // 4
#define DIRECTION_DOWN        0b00001000  // 8
#define DIRECTION_LEFT_UP     (DIRECTION_LEFT | DIRECTION_UP)   // 0b00000101 = 5
#define DIRECTION_LEFT_DOWN   (DIRECTION_LEFT | DIRECTION_DOWN) // 0b00001001 = 9
#define DIRECTION_RIGHT_UP    (DIRECTION_RIGHT | DIRECTION_UP)  // 0b00000110 = 6
#define DIRECTION_RIGHT_DOWN  (DIRECTION_RIGHT | DIRECTION_DOWN)// 0b00001010 = 10

#define MIN_CAMERA_POS_X       0
#define MIN_CAMERA_POS_Y       0

#define NONE_TILE_INDEX        0
#define SOLID_TILE_INDEX       1
#define BONUS_TILE_INDEX       2
#define M_PLATFORM_TILE_INDEX  11 // оставляем пропуски для определения смещения на 8 пикселей в любую сторону по горизонтали
#define STAIRS_TILE_INDEX      20

#if (DEBUG_COLLISIONS)
    extern Sprite* tileCursorsR[12];
    extern Sprite* playerCursor;
#endif

#define masPointer2(mas, i, j) (*(mas + (i * mapWTiles) + j))

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
void engine_drawDebugInt(const char* text, s32 num, u16 x, u16 y);
void engine_drawDebugUInt(const char* text, u32 num, u16 x, u16 y);
void engine_drawInt(s16 num, u16 x, u16 y, u16 len);
void engine_drawFix32(const char* text, f32 num, u16 x, u16 y);

u8 engine_getTileIndex(u8* collisions, s16 xTile, s16 yTile);
bool engine_checkMovingTileIndex(u8 index, u8 middleIndex);
s16 engine_getMovingTileShift(u8 *collisions, s16 xTile, s16 yTile, u8 middleIndex);
bool engine_isTileSolid(u8* collisions, s16 xTile, s16 yTile);

AABB engine_checkMapArea(u8* collisions, AABB aabb);
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
void engine_checkCollisions(AABB aabb, u8* collisionsMap, u8 direction, u8* left, u8* right, u8* top, u8* bottom);
u8 engine_getIntersectionLen(AxisLine_s16 a, AxisLine_s16 b);

void engine_showFPS(u16 asFloat, u16 x, u16 y);
void engine_showCPULoad(u16 x, u16 y);
