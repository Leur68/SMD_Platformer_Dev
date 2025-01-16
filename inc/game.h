#include <global.h>


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

#define GAME_MAX_OBJECTS          100

void stateGame_init();
void stateGame_joyInit();

void stateGame_process();
void stateGame_update();
void stateGame_release();
void stateGame_initObject();
void stateGame_initObjectSprite();
void stateGame_onUpdateObject();
void stateGame_onUpdateObjectInViewport();
void stateGame_onObjectCollidesWithPlayerInViewport();

void stateGame_scroll();
void stateGame_tooglePause();

void stateGame_joyHandlerBefore();
void stateGame_joyHandlerAfter();

void stateGame_joyHandler3(u16 joy, u16 changed, u16 state);
void stateGame_joyHandler6(u16 joy, u16 changed, u16 state);

void stateGame_buttonUpHold();
void stateGame_buttonDownHold();
void stateGame_buttonLeftHold();
void stateGame_buttonRightHold();

void stateGame_buttonUpPress();
void stateGame_buttonDownPress();
void stateGame_buttonLeftPress();
void stateGame_buttonRightPress();

void stateGame_buttonUpRelease();
void stateGame_buttonDownRelease();
void stateGame_buttonLeftRelease();
void stateGame_buttonRightRelease();

void stateGame_buttonStart();
void stateGame_buttonMode();

void stateGame_buttonX();
void stateGame_buttonY();
void stateGame_buttonZ();
void stateGame_buttonA();
void stateGame_buttonB();
void stateGame_buttonC();

// 8 bit do controller
void stateGame_buttonL();
void stateGame_buttonR();