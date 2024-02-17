#include "../inc/game.h"

bool paused = FALSE;
bool xyzButtons = FALSE;

// Joy

void stateLevel1_joyInit() {
    JOY_setEventHandler(&stateLevel1_joyHandler3);
}

void stateLevel1_updateDPad() {
    s16 state = JOY_readJoypad(JOY_1);
    if (state & BUTTON_UP) {
        stateLevel1_buttonUp();
    }
    if (state & BUTTON_DOWN) {
        stateLevel1_buttonDown();
    }
    if (state & BUTTON_LEFT) {
        stateLevel1_buttonLeft();
    }
    if (state & BUTTON_RIGHT) {
        stateLevel1_buttonRight();
    }
}

void stateLevel1_joyHandler3(u16 joy, u16 changed, u16 state) {
    if(state & BUTTON_START) {
        stateLevel1_buttonStart();
    }
    if (paused) {
        return;
    }
    stateLevel1_joyHandlerBefore();
    if (state & BUTTON_A) {
        stateLevel1_buttonA();
    }
    if (state & BUTTON_B) {
        stateLevel1_buttonB();
    }
    if (state & BUTTON_C) {
        stateLevel1_buttonC();
    }
    stateLevel1_joyHandlerAfter();
}

void stateLevel1_joyHandler6(u16 joy, u16 changed, u16 state) {
    if(state & BUTTON_START) {
        stateLevel1_buttonStart();
    }
    if (paused) {
        return;
    }
    stateLevel1_joyHandlerBefore();
    if (state & BUTTON_X) {
        stateLevel1_buttonX();
    }
    if (state & BUTTON_Y) {
        stateLevel1_buttonY();
    }
    if (state & BUTTON_Z) {
        stateLevel1_buttonZ();
    }
    if (state & BUTTON_A) {
        stateLevel1_buttonA();
    }
    if (state & BUTTON_B) {
        stateLevel1_buttonB();
    }
    if (state & BUTTON_C) {
        stateLevel1_buttonC();
    }
    stateLevel1_joyHandlerAfter();
}

// Process

void stateLevel1_process() {
    stateLevel1_init();

    while(currentState == STATE_PLAY_1) {
        if (!paused) {
            stateLevel1_updateDPad();

            stateLevel1_update();

            SPR_update();
        }
        VDP_showFPS(0);

        SYS_doVBlankProcess();
    }

    stateLevel1_release();
}