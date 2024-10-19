#include "../inc/game.h"

// Joy

void stateIntro_joyHandler(u16 joy, u16 changed, u16 state) {
    if (changed & state & BUTTON_START) {
        stateIntro_buttonStart();
    }
}

// Process

void stateIntro_process() {
    stateIntro_init();

    while(currentState == STATE_INTRO) {
        stateIntro_update();

        SPR_update();
        SYS_doVBlankProcess();
    }

    // release all resources
    JOY_setEventHandler(NULL);
    VDP_init();
}