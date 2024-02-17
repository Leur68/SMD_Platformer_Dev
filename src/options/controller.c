#include "../inc/game.h"

// Joy

void stateOptions_joyInit() {
    u16 joy;
    if (JOY_getPortType(PORT_1) == PORT_TYPE_PAD) {
        joy = JOY_1;
    } else if (JOY_getPortType(PORT_2) == PORT_TYPE_PAD) {
        joy = JOY_2;
    }
    JOY_setEventHandler(&stateOptions_joyHandler3);
}

void stateOptions_joyHandler3(u16 joy, u16 changed, u16 state) {
    if(state & BUTTON_START) {
        stateOptions_buttonStart();
    }
    stateOptions_joyHandlerBefore();
    if (state & BUTTON_UP) {
        stateOptions_buttonUp();
    }
    if(state & BUTTON_DOWN) {
        stateOptions_buttonDown();
    }
    if (state & BUTTON_LEFT) {
        stateOptions_buttonLeft();
    }
    if(state & BUTTON_RIGHT) {
        stateOptions_buttonRight();
    }
    if(state & BUTTON_A) {
        stateOptions_buttonA();
    }
    if(state & BUTTON_B) {
        stateOptions_buttonB();
    }
    if(state & BUTTON_C) {
        stateOptions_buttonC();
    }
    stateOptions_joyHandlerAfter();
}

// Process

void stateOptions_process() {
    stateOptions_init();

    while(currentState == STATE_OPTIONS) {
        stateOptions_update();

        SPR_update();
        SYS_doVBlankProcess();
    }

    stateOptions_release();
}