#include <genesis.h>

// Палитры
#define TEXT_PALETTE              PAL0
#define BACKGROUND_PALETTE        PAL1
#define GROUND_PALETTE            PAL2
#define PLAYER_PALETTE            PAL3
#define DEBUG_PALETTE             PAL3

// Плейны
#define GROUND_PLANE              BG_A
#define BACKGROUND_PLANE          BG_B
#define TEXT_PLANE                WINDOW

#define MAX_VELOCITY              3.5
#define ACCELERATION              0.125
#define DECELERATION              0.125

#define GRAVITY                   3
#define GRAVITY_ACCELERATION      0.125

#define JUMP                      5

#define MAP_WIDTH_TILES           100  // Ширина карты в тайлах
#define MAP_HEIGHT_TILES          100  // Высота карты в тайлах
#define MAP_WIDTH                 (MAP_WIDTH_TILES * 8)  // Ширина карты в пикселях
#define MAP_HEIGHT                (MAP_HEIGHT_TILES * 8)  // Высота карты в пикселях
#define MAP_MAX_CAMERA_POS_X      (MAP_WIDTH - SCREEN_WIDTH)
#define MAP_MAX_CAMERA_POS_Y      (MAP_HEIGHT - SCREEN_HEIGHT)

#define BACK_WIDTH_TILES          50  // Ширина фона в тайлах
#define BACK_HEIGHT_TILES         50  // Высота фона в тайлах
#define BACK_WIDTH                (BACK_WIDTH_TILES * 8)  // Ширина фона в пикселях
#define BACK_HEIGHT               (BACK_HEIGHT_TILES * 8)  // Высота фона в пикселях
#define BACK_MAX_CAMERA_POS_X     (BACK_WIDTH - SCREEN_WIDTH)  // 
#define BACK_MAX_CAMERA_POS_Y     (BACK_HEIGHT - SCREEN_HEIGHT)  // 

#define PARALLAX_RATIO_X          FIX32(BACK_WIDTH - SCREEN_WIDTH) / FIX32(MAP_WIDTH - SCREEN_WIDTH)
#define PARALLAX_RATIO_Y          FIX32(BACK_HEIGHT - SCREEN_HEIGHT) / FIX32(MAP_HEIGHT - SCREEN_HEIGHT)

#define NOT_SCROLLABLE_MIN_X      118
#define NOT_SCROLLABLE_MAX_X      202
#define NOT_SCROLLABLE_MIN_Y      70
#define NOT_SCROLLABLE_MAX_Y      154

#define LEVEL1_MAX_OBJECTS        100

void stateLevel1_init();
void stateLevel1_joyInit();

void stateLevel1_process();
void stateLevel1_update();
void stateLevel1_initObject();
void stateLevel1_initObjectSprite();
void stateLevel1_onUpdateObject();
void stateLevel1_onUpdateObjectInViewport();
void stateLevel1_onObjectCollidesWithCharacterInViewport();

void stateLevel1_scroll();
void stateLevel1_tooglePause();

void stateLevel1_joyHandlerBefore();
void stateLevel1_joyHandlerAfter();

void stateLevel1_joyHandler3(u16 joy, u16 changed, u16 state);
void stateLevel1_joyHandler6(u16 joy, u16 changed, u16 state);

void stateLevel1_buttonUpHold();
void stateLevel1_buttonDownHold();
void stateLevel1_buttonLeftHold();
void stateLevel1_buttonRightHold();

void stateLevel1_buttonUpPress();
void stateLevel1_buttonDownPress();
void stateLevel1_buttonLeftPress();
void stateLevel1_buttonRightPress();

void stateLevel1_buttonUpRelease();
void stateLevel1_buttonDownRelease();
void stateLevel1_buttonLeftRelease();
void stateLevel1_buttonRightRelease();

void stateLevel1_buttonStart();
void stateLevel1_buttonMode();

void stateLevel1_buttonX();
void stateLevel1_buttonY();
void stateLevel1_buttonZ();
void stateLevel1_buttonA();
void stateLevel1_buttonB();
void stateLevel1_buttonC();