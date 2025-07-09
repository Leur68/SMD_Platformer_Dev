#include "global.h"

void stateGameOver_joyInit() {
    JOY_setEventHandler(&stateGameOver_joyChangedHandler);
}

void stateGameOver_joyChangedHandler(u16 joy, u16 changed, u16 state) {
    if (changed & state & BUTTON_START) {
        stateGameOver_buttonStartPress();
    }
    if (paused) {
        return;
    }
    stateGameOver_joyHandlerBefore();

    if (changed & state & BUTTON_LEFT) {
        stateGameOver_buttonLeftPress();
    }
    if (changed & state & BUTTON_RIGHT) {
        stateGameOver_buttonRightPress();
    }
    if (changed & state & BUTTON_UP) {
        stateGameOver_buttonUpPress();
    }
    if (changed & state & BUTTON_DOWN) {
        stateGameOver_buttonDownPress();
    }
    if (changed & state & BUTTON_A) {
        stateGameOver_buttonAPress();
    }
    if (changed & state & BUTTON_B) {
        stateGameOver_buttonBPress();
    }
    if (changed & state & BUTTON_C) {
        stateGameOver_buttonCPress();
    }
    if (changed & state & BUTTON_MODE) {
        stateGameOver_buttonModePress();
    }

    stateGameOver_joyHandlerAfter();
}

void stateGameOver_process() {
    stateGameOver_init();

    while (currentState == STATE_GAMEOVER) {
        stateGameOver_update();

        SPR_update();
        SPR_defragVRAM();

        SYS_doVBlankProcess();
    }

    // release all resources
    JOY_setEventHandler(NULL);
    VDP_init();
    stateGameOver_release();
}