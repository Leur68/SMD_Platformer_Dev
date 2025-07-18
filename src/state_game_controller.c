#include "global.h"

void stateGame_joyInit() {
    JOY_setEventHandler(&stateGame_joyChangedHandler);
}

void stateGame_joyChangedHandler(u16 joy, u16 changed, u16 state) {
    if (changed & state & BUTTON_START) {
        stateGame_buttonStartPress();
    }
    if (!(state & BUTTON_START) && (changed & BUTTON_START)) {
        stateGame_buttonStartRelease();
    }
    if (paused) {
        return;
    }
    stateGame_joyHandlerBefore();

    if (changed & state & BUTTON_LEFT) {
        stateGame_buttonLeftPress();
    }
    if (changed & state & BUTTON_RIGHT) {
        stateGame_buttonRightPress();
    }
    if (changed & state & BUTTON_UP) {
        stateGame_buttonUpPress();
    }
    if (changed & state & BUTTON_DOWN) {
        stateGame_buttonDownPress();
    }
    if (changed & state & BUTTON_X) {
        stateGame_buttonXPress();
    }
    if (changed & state & BUTTON_Y) {
        stateGame_buttonYPress();
    }
    if (changed & state & BUTTON_Z) {
        stateGame_buttonZPress();
    }
    if (changed & state & BUTTON_A) {
        stateGame_buttonAPress();
    }
    if (changed & state & BUTTON_B) {
        stateGame_buttonBPress();
    }
    if (changed & state & BUTTON_C) {
        stateGame_buttonCPress();
    }
    if (changed & state & BUTTON_MODE) {
        stateGame_buttonModePress();
    }

    if (!(state & BUTTON_LEFT) && (changed & BUTTON_LEFT)) {
        stateGame_buttonLeftRelease();
    }
    if (!(state & BUTTON_RIGHT) && (changed & BUTTON_RIGHT)) {
        stateGame_buttonRightRelease();
    }
    if (!(state & BUTTON_UP) && (changed & BUTTON_UP)) {
        stateGame_buttonUpRelease();
    }
    if (!(state & BUTTON_DOWN) && (changed & BUTTON_DOWN)) {
        stateGame_buttonDownRelease();
    }
    if (!(state & BUTTON_X) && (changed & BUTTON_X)) {
        stateGame_buttonXRelease();
    }
    if (!(state & BUTTON_Y) && (changed & BUTTON_Y)) {
        stateGame_buttonYRelease();
    }
    if (!(state & BUTTON_Z) && (changed & BUTTON_Z)) {
        stateGame_buttonZRelease();
    }
    if (!(state & BUTTON_A) && (changed & BUTTON_A)) {
        stateGame_buttonARelease();
    }
    if (!(state & BUTTON_B) && (changed & BUTTON_B)) {
        stateGame_buttonBRelease();
    }
    if (!(state & BUTTON_C) && (changed & BUTTON_C)) {
        stateGame_buttonCRelease();
    }
    if (!(state & BUTTON_MODE) && (changed & BUTTON_MODE)) {
        stateGame_buttonModeRelease();
    }

    stateGame_joyHandlerAfter();
}

void stateGame_joyHoldingHandler() {
    u16 state = JOY_readJoypad(JOY_ALL);

    if (state & BUTTON_START) {
        stateGame_buttonStartHold();
    }

    stateGame_joyHandlerBefore();

    if (state & BUTTON_LEFT) {
        if (state & BUTTON_Z) {
            stateGame_buttonZplusLeftHold();
        } else {
            stateGame_buttonLeftHold();
        }
    }
    if (state & BUTTON_RIGHT) {
        if (state & BUTTON_Z) {
            stateGame_buttonZplusRightHold();
        } else {
            stateGame_buttonRightHold();
        }
    }
    if (state & BUTTON_UP) {
        if (state & BUTTON_Z) {
            stateGame_buttonZplusUpHold();
        } else {
            stateGame_buttonUpHold();
        }
    }
    if (state & BUTTON_DOWN) {
        if (state & BUTTON_Z) {
            stateGame_buttonZplusDownHold();
        } else {
            stateGame_buttonDownHold();
        }
    }
    
    if (state & BUTTON_X) {
        stateGame_buttonXHold();
    }
    if (state & BUTTON_Y) {
        stateGame_buttonYHold();
    }
    if (state & BUTTON_Z) {
        stateGame_buttonZHold();
    }
    if (state & BUTTON_A) {
        stateGame_buttonAHold();
    }
    if (state & BUTTON_B) {
        stateGame_buttonBHold();
    }
    if (state & BUTTON_C) {
        stateGame_buttonCHold();
    }
    if (state & BUTTON_MODE) {
        stateGame_buttonModeHold();
    }
    stateGame_joyHandlerAfter();
}

void stateGame_process() {
    stateGame_init();

    while (currentState == STATE_GAME) {
        if (!paused) {
            stateGame_joyHoldingHandler();

            stateGame_update();

#if DEBUG_SHOW_FPS
            VDP_showFPS(true, 0, 0);
#endif
#if DEBUG_SHOW_CPU
            VDP_showCPULoad(5, 0);
#endif
#if DEBUG_SHOW_FREE_RAM
            drawInt(MEM_getFree(), 9, 0, 6);
#endif

            SPR_update();
            SPR_defragVRAM();
        }

        SYS_doVBlankProcess();
    }

    // release all resources
    JOY_setEventHandler(NULL);
    VDP_init();
    stateGame_release();
}