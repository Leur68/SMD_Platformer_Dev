#include <global.h>

#define currScreenPosInScrollableX     player->screenPos.x >= NOT_SCROLLABLE_MIN_X && player->screenPos.x + PLAYER_WIDTH <= NOT_SCROLLABLE_MAX_X
#define nextScreenPosMinX              player->screenPos.x + player->movedPixels.x
#define nextScreenPosMaxX              player->screenPos.x + PLAYER_WIDTH + player->movedPixels.x
#define nextScreenPosInNotScrollableX  nextScreenPosMinX < NOT_SCROLLABLE_MIN_X || nextScreenPosMaxX > NOT_SCROLLABLE_MAX_X
#define currScreenPosInScrollableY     player->screenPos.y >= NOT_SCROLLABLE_MIN_Y && player->screenPos.y + PLAYER_HEIGHT <= NOT_SCROLLABLE_MAX_Y
#define nextScreenPosMinY              player->screenPos.y + player->movedPixels.y
#define nextScreenPosMaxY              player->screenPos.y + PLAYER_HEIGHT + player->movedPixels.y
#define nextScreenPosInNotScrollableY  nextScreenPosMinY < NOT_SCROLLABLE_MIN_Y || nextScreenPosMaxY > NOT_SCROLLABLE_MAX_Y

void camera_init();
void camera_update();