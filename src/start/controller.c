#include "../inc/game.h"

// Joy

void stateStart_joyHandler(u16 joy, u16 changed, u16 state) {
    if (changed & state & BUTTON_UP) {
        stateStart_buttonUp();
    } else if(changed & state & BUTTON_DOWN) {
        stateStart_buttonDown();
    } else if(changed & state & BUTTON_START) {
        stateStart_buttonStart();
    }
}

// Process

void stateStart_process() {
    stateStart_init();

    while(currentState == STATE_START) {
        stateStart_update();
        
        SPR_update();
        SYS_doVBlankProcess();
    }

    // release all resources
    JOY_setEventHandler(NULL);
    VDP_init();
}