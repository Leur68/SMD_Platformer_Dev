#include "../inc/game.h"

// Joy

void stateLevel1_joyInit() {
    JOY_setEventHandler(&stateLevel1_joyHandler6);
}

void stateLevel1_updateDPad() {
    s16 state = JOY_readJoypad(JOY_1);
    if (state & BUTTON_UP) {
        stateLevel1_buttonUpHold();
    }
    if (state & BUTTON_DOWN) {
        stateLevel1_buttonDownHold();
    }
    if (state & BUTTON_LEFT) {
        stateLevel1_buttonLeftHold();
    }
    if (state & BUTTON_RIGHT) {
        stateLevel1_buttonRightHold();
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

    if (state & BUTTON_UP) {
        stateLevel1_buttonUpPress();
    }
    if (state & BUTTON_DOWN) {
        stateLevel1_buttonDownPress();
    }
    if (state & BUTTON_LEFT) {
        stateLevel1_buttonLeftPress();
    }
    if (state & BUTTON_RIGHT) {
        stateLevel1_buttonRightPress();
    }

    if (!(state & BUTTON_UP) && (changed & BUTTON_UP)) {
        stateLevel1_buttonUpRelease();
    }
    if (!(state & BUTTON_DOWN) && (changed & BUTTON_DOWN)) {
        stateLevel1_buttonDownRelease();
    }
    if (!(state & BUTTON_LEFT) && (changed & BUTTON_LEFT)) {
        stateLevel1_buttonLeftRelease();
    }
    if (!(state & BUTTON_RIGHT) && (changed & BUTTON_RIGHT)) {
        stateLevel1_buttonRightRelease();
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

void stateLevel1_joyHandler6(u16 joy, u16 changed, u16 state) {
    if(state & BUTTON_START) {
        stateLevel1_buttonStart();
    }
    if (paused) {
        return;
    }
    stateLevel1_joyHandlerBefore();

    if (state & BUTTON_UP) {
        stateLevel1_buttonUpPress();
    }
    if (state & BUTTON_DOWN) {
        stateLevel1_buttonDownPress();
    }
    if (state & BUTTON_LEFT) {
        stateLevel1_buttonLeftPress();
    }
    if (state & BUTTON_RIGHT) {
        stateLevel1_buttonRightPress();
    }

    if (!(state & BUTTON_UP) && (changed & BUTTON_UP)) {
        stateLevel1_buttonUpRelease();
    }
    if (!(state & BUTTON_DOWN) && (changed & BUTTON_DOWN)) {
        stateLevel1_buttonDownRelease();
    }
    if (!(state & BUTTON_LEFT) && (changed & BUTTON_LEFT)) {
        stateLevel1_buttonLeftRelease();
    }
    if (!(state & BUTTON_RIGHT) && (changed & BUTTON_RIGHT)) {
        stateLevel1_buttonRightRelease();
    }

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
    if (state & BUTTON_MODE) {
        stateLevel1_buttonMode();
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
        engine_showFPS(0, 0, 0);

        SYS_doVBlankProcess();
    }

    stateLevel1_release();
}