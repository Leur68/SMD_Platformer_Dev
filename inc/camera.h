#include <global.h>

#define currScreenPosInScrollableX(screenX)               (screenX >= NOT_SCROLLABLE_MIN_X) && ((screenX + PLAYER_WIDTH) <= NOT_SCROLLABLE_MAX_X)
#define nextScreenPosMinX(screenX)                        (screenX + player->movedPixels.x)
#define nextScreenPosMaxX(screenX)                        (screenX + PLAYER_WIDTH + player->movedPixels.x)
#define nextScreenPosInNotScrollableX(screenX)            ((nextScreenPosMinX(screenX) < NOT_SCROLLABLE_MIN_X) || (nextScreenPosMaxX(screenX) > NOT_SCROLLABLE_MAX_X))

#define currScreenPosInScrollableY(screenY)               ((screenY >= NOT_SCROLLABLE_MIN_Y) && ((screenY + PLAYER_HEIGHT) <= NOT_SCROLLABLE_MAX_Y))
#define nextScreenPosMinY(screenY)                        (screenY + player->movedPixels.y)
#define nextScreenPosMaxY(screenY)                        (screenY + PLAYER_HEIGHT + player->movedPixels.y)
#define nextScreenPosInNotScrollableY(screenY)            ((nextScreenPosMinY(screenY) < NOT_SCROLLABLE_MIN_Y) || (nextScreenPosMaxY(screenY) > NOT_SCROLLABLE_MAX_Y))

#define isObjectVisible(screenX, screenY, width, height)  (((screenX + width) >= 0 && (screenX) < (SCREEN_WIDTH)) && ((screenY + height) >= 0 && (screenY) < (SCREEN_HEIGHT)))

void camera_init();
void camera_update();
void camera_mustScrollByX(s16 v);
void camera_mustScrollByY(s16 v);