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

// Write the value into this field, taking only the lowest 3 bits of value
// Mask 0x7 for 3 bits: 0b111, or 7 in decimal representation
#define setThreeBitField(num, value, bitShift)   \
    *num |= ((value & 0x7) << bitShift);

#define setBit(num, bitPosition)   \
    *num |= (1 << bitPosition);
    
#define clearBit(num, bitPosition) \
    *(num) &= ~(1 << (bitPosition))