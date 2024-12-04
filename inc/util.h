#include <global.h>

#define SLOW_FADE              50
#define NORMAL_FADE            30
#define FAST_FADE              10

#define masPointer2(mas, i, j) (*(mas + (i * mapWTiles) + j))

void global_init();
void disableSoftReset(bool resetType);
void fadeInImage(Image img, u16 numFrame);
void fadeInPalette(const u16 * pal, u16 numFrame);
void fadeInScreen(u16 numFrame);
void fadeOutScreen(u16 numFrame);