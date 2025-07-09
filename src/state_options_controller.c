#include "global.h"

void stateOptions_joyInit() {
    JOY_setEventHandler(&stateOptions_joyHandler);
}

void stateOptions_joyHandler(u16 joy, u16 changed, u16 state) {
    if (changed & state & BUTTON_START) {
        stateOptions_buttonStart();
    }
    stateOptions_joyHandlerBefore();
    if (changed & state & BUTTON_UP) {
        stateOptions_buttonUp();
    }
    if (changed & state & BUTTON_DOWN) {
        stateOptions_buttonDown();
    }
    if (changed & state & BUTTON_LEFT) {
        stateOptions_buttonLeft();
    }
    if (changed & state & BUTTON_RIGHT) {
        stateOptions_buttonRight();
    }
    if (changed & state & BUTTON_A) {
        stateOptions_buttonA();
    }
    if (changed & state & BUTTON_B) {
        stateOptions_buttonB();
    }
    if (changed & state & BUTTON_C) {
        stateOptions_buttonC();
    }
    stateOptions_joyHandlerAfter();
}

void stateOptions_process() {
    stateOptions_init();

    while (currentState == STATE_OPTIONS) {
        stateOptions_update();

        SPR_update();
        SPR_defragVRAM();
        SYS_doVBlankProcess();
    }

    // release all resources
    JOY_setEventHandler(NULL);
    VDP_init();
    stateOptions_release();
}