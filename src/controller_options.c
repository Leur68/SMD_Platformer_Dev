#include "../inc/global.h"

void stateOptions_joyInit() {
    JOY_setEventHandler(&stateOptions_joyHandler);
}

void stateOptions_joyHandler(u16 joy, u16 changed, u16 state) {
    if (state & changed & BUTTON_START) {
        stateOptions_buttonStart();
    }
    stateOptions_joyHandlerBefore();
    if (state & changed & BUTTON_UP) {
        stateOptions_buttonUp();
    }
    if (state & changed & BUTTON_DOWN) {
        stateOptions_buttonDown();
    }
    if (state & changed & BUTTON_LEFT) {
        stateOptions_buttonLeft();
    }
    if (state & changed & BUTTON_RIGHT) {
        stateOptions_buttonRight();
    }
    if (state & changed & BUTTON_A) {
        stateOptions_buttonA();
    }
    if (state & changed & BUTTON_B) {
        stateOptions_buttonB();
    }
    if (state & changed & BUTTON_C) {
        stateOptions_buttonC();
    }
    stateOptions_joyHandlerAfter();
}

void stateOptions_process() {
    stateOptions_init();

    while(currentState == STATE_OPTIONS) {
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