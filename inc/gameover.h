#include <global.h>

void stateGameOver_init();
void stateGameOver_joyInit();

void stateGameOver_process();
void stateGameOver_update();
void stateGameOver_release();

void stateGameOver_joyHandlerBefore();
void stateGameOver_joyHandlerAfter();

void stateGameOver_joyChangedHandler(u16 joy, u16 changed, u16 state);

void stateGameOver_buttonUpPress();
void stateGameOver_buttonDownPress();
void stateGameOver_buttonLeftPress();
void stateGameOver_buttonRightPress();
void stateGameOver_buttonAPress();
void stateGameOver_buttonBPress();
void stateGameOver_buttonCPress();
void stateGameOver_buttonStartPress();
void stateGameOver_buttonModePress();