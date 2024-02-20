#include <genesis.h>

#define GROUND_PALETTE            PAL0
#define PLAYER_PALETTE            PAL3

#define GROUND_PLANE              BG_B

#define MAX_VELOCITY              0.25
#define ACCELERATION              0.0625

#define NORMAL_VELOCITY           1
#define NORMAL_GRAVITY            1

#define MAP_WIDTH_TILES           100  // Ширина карты в тайлах
#define MAP_HEIGHT_TILES          100  // Высота карты в тайлах
#define MAP_WIDTH                 MAP_WIDTH_TILES * 8  // Ширина карты в пикселях
#define MAP_HEIGHT                MAP_HEIGHT_TILES * 8  // Высота карты в пикселях
#define MAP_SHIFT_X_MAX           MAP_WIDTH - SCREEN_WIDTH  // 
#define MAP_SHIFT_Y_MAX           MAP_HEIGHT - SCREEN_HEIGHT  // 
#define MAP_OVERHEIGHT            MAP_HEIGHT - SCREEN_HEIGHT // 

#define SCREEN_MIN_X              160
#define SCREEN_MAX_X              160
#define SCREEN_MIN_Y              112
#define SCREEN_MAX_Y              112

extern bool paused;
extern bool xyzButtons;
extern u8 collisions1[MAP_HEIGHT_TILES][MAP_WIDTH_TILES];
extern s16 mapShiftX;
extern s16 mapShiftY;

void stateLevel1_init();
void stateLevel1_release();
void stateLevel1_joyInit();
void stateLevel1_joyHandlerBefore();
void stateLevel1_joyHandlerAfter();
void stateLevel1_joyHandler3(u16 joy, u16 changed, u16 state);
void stateLevel1_joyHandler6(u16 joy, u16 changed, u16 state);
void stateLevel1_buttonUp();
void stateLevel1_buttonDown();
void stateLevel1_buttonLeft();
void stateLevel1_buttonRight();
void stateLevel1_buttonStart();
void stateLevel1_buttonX();
void stateLevel1_buttonY();
void stateLevel1_buttonZ();
void stateLevel1_buttonA();
void stateLevel1_buttonB();
void stateLevel1_buttonC();
void stateLevel1_process();
void stateLevel1_scrollMap();
void stateLevel1_update();
void stateLevel1_tooglePause();