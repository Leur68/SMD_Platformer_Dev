#include <global.h>

#define SLOW_FADE              50
#define NORMAL_FADE            30
#define FAST_FADE              10

#define mapPointerGet(mas, x, y) (*(mas + (y * mapWTiles) + x))
#define mapPointerSet(mas, x, y, v) (*(mas + (y * mapWTiles) + x) = v)

void global_init();
void disableSoftReset(bool resetType);
void fadeInImage(Image img, u16 numFrame);
void fadeInPalette(const u16 *pal, u16 numFrame);
void fadeInScreen(u16 numFrame);
void fadeOutScreen(u16 numFrame);
void drawInt(s32 num, u16 x, u16 y, u16 len);
void drawFix(f32 num, u16 x, u16 y, u16 len);
void setThreeBitField(u16 *num, u8 value, u8 bitShift);
void setBit(u16 *num, u8 value, u8 bitPosition);