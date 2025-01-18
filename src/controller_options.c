#include "../inc/global.h"

// Joy init
void stateOptions_joyInit() {
    JOY_setEventHandler(&stateOptions_joyHandler);
}

// Joy handler
void stateOptions_joyHandler(u16 joy, u16 changed, u16 state) {
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

// Process stage
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