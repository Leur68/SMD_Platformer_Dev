#include <global.h>


#define MAX_VELOCITY              3.5
#define ACCELERATION              0.125
#define DECELERATION              0.125

#define GRAVITY                   3
#define GRAVITY_ACCELERATION      0.125

#define JUMP                      5

#define MAP_WIDTH_TILES           100  // Map width in tiles
#define MAP_HEIGHT_TILES          100  // Map height in tiles
#define MAP_WIDTH                 (MAP_WIDTH_TILES * 8)  // Map width in pixels
#define MAP_HEIGHT                (MAP_HEIGHT_TILES * 8)  // Map height in pixels
#define MAP_MAX_CAMERA_POS_X      (MAP_WIDTH - SCREEN_WIDTH)
#define MAP_MAX_CAMERA_POS_Y      (MAP_HEIGHT - SCREEN_HEIGHT)

#define BACK_WIDTH_TILES          50  // Background width in tiles
#define BACK_HEIGHT_TILES         50  // Background height in tiles
#define BACK_WIDTH                (BACK_WIDTH_TILES * 8)  // Background width in pixels
#define BACK_HEIGHT               (BACK_HEIGHT_TILES * 8)  // Background height in pixels
#define BACK_MAX_CAMERA_POS_X     (BACK_WIDTH - SCREEN_WIDTH)
#define BACK_MAX_CAMERA_POS_Y     (BACK_HEIGHT - SCREEN_HEIGHT)

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

void stateGame_joyChangedHandler(u16 joy, u16 changed, u16 state);
void stateGame_joyHoldingHandler();

void stateGame_buttonUpHold();
void stateGame_buttonDownHold();
void stateGame_buttonLeftHold();
void stateGame_buttonRightHold();
void stateGame_buttonXHold();
void stateGame_buttonYHold();
void stateGame_buttonZHold();
void stateGame_buttonAHold();
void stateGame_buttonBHold();
void stateGame_buttonCHold();
void stateGame_buttonStartHold();
void stateGame_buttonModeHold();

void stateGame_buttonUpPress();
void stateGame_buttonDownPress();
void stateGame_buttonLeftPress();
void stateGame_buttonRightPress();
void stateGame_buttonXPress();
void stateGame_buttonYPress();
void stateGame_buttonZPress();
void stateGame_buttonAPress();
void stateGame_buttonBPress();
void stateGame_buttonCPress();
void stateGame_buttonStartPress();
void stateGame_buttonModePress();

void stateGame_buttonUpRelease();
void stateGame_buttonDownRelease();
void stateGame_buttonLeftRelease();
void stateGame_buttonRightRelease();
void stateGame_buttonXRelease();
void stateGame_buttonYRelease();
void stateGame_buttonZRelease();
void stateGame_buttonARelease();
void stateGame_buttonBRelease();
void stateGame_buttonCRelease();
void stateGame_buttonStartRelease();
void stateGame_buttonModeRelease();