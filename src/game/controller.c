#include "../inc/global.h"

// Joy

void stateGame_joyInit() {
    JOY_setEventHandler(&stateGame_joyHandler6);
}

void stateGame_updateDPad() {
    u16 state = JOY_readJoypad(JOY_1);
    if (state & BUTTON_UP) {
        stateGame_buttonUpHold();
    }
    if (state & BUTTON_DOWN) {
        stateGame_buttonDownHold();
    }
    if (state & BUTTON_LEFT) {
        stateGame_buttonLeftHold();
    }
    if (state & BUTTON_RIGHT) {
        stateGame_buttonRightHold();
    }
}

void stateGame_joyHandler3(u16 joy, u16 changed, u16 state) {
    if(state & BUTTON_START) {
        stateGame_buttonStart();
    }
    if (paused) {
        return;
    }
    stateGame_joyHandlerBefore();

    if (state & BUTTON_UP) {
        stateGame_buttonUpPress();
    }
    if (state & BUTTON_DOWN) {
        stateGame_buttonDownPress();
    }
    if (state & BUTTON_LEFT) {
        stateGame_buttonLeftPress();
    }
    if (state & BUTTON_RIGHT) {
        stateGame_buttonRightPress();
    }

    if (!(state & BUTTON_UP) && (changed & BUTTON_UP)) {
        stateGame_buttonUpRelease();
    }
    if (!(state & BUTTON_DOWN) && (changed & BUTTON_DOWN)) {
        stateGame_buttonDownRelease();
    }
    if (!(state & BUTTON_LEFT) && (changed & BUTTON_LEFT)) {
        stateGame_buttonLeftRelease();
    }
    if (!(state & BUTTON_RIGHT) && (changed & BUTTON_RIGHT)) {
        stateGame_buttonRightRelease();
    }

    if (state & BUTTON_A) {
        stateGame_buttonA();
    }
    if (state & BUTTON_B) {
        stateGame_buttonB();
    }
    if (state & BUTTON_C) {
        stateGame_buttonC();
    }
    stateGame_joyHandlerAfter();
}

void stateGame_joyHandler6(u16 joy, u16 changed, u16 state) {
    if(state & BUTTON_START) {
        stateGame_buttonStart();
    }
    if (paused) {
        return;
    }
    stateGame_joyHandlerBefore();

    if (state & BUTTON_UP) {
        stateGame_buttonUpPress();
    }
    if (state & BUTTON_DOWN) {
        stateGame_buttonDownPress();
    }
    if (state & BUTTON_LEFT) {
        stateGame_buttonLeftPress();
    }
    if (state & BUTTON_RIGHT) {
        stateGame_buttonRightPress();
    }

    if (!(state & BUTTON_UP) && (changed & BUTTON_UP)) {
        stateGame_buttonUpRelease();
    }
    if (!(state & BUTTON_DOWN) && (changed & BUTTON_DOWN)) {
        stateGame_buttonDownRelease();
    }
    if (!(state & BUTTON_LEFT) && (changed & BUTTON_LEFT)) {
        stateGame_buttonLeftRelease();
    }
    if (!(state & BUTTON_RIGHT) && (changed & BUTTON_RIGHT)) {
        stateGame_buttonRightRelease();
    }

    if (state & BUTTON_X) {
        stateGame_buttonX();
    }
    if (state & BUTTON_Y) {
        stateGame_buttonY();
    }
    if (state & BUTTON_Z) {
        stateGame_buttonZ();
    }
    if (state & BUTTON_A) {
        stateGame_buttonA();
    }
    if (state & BUTTON_B) {
        stateGame_buttonB();
    }
    if (state & BUTTON_C) {
        stateGame_buttonC();
    }
    if (state & BUTTON_MODE) {
        stateGame_buttonMode();
    }
    stateGame_joyHandlerAfter();
}

// Process

void stateGame_process() {
    stateGame_init();

    while(currentState == STATE_GAME) {
        if (!paused) {
            stateGame_updateDPad();

            stateGame_update();
        
            VDP_showFPS(true, 0, 0);
            VDP_showCPULoad(5, 0);

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