#include "global.h"

void stateStart_joyHandler(u16 joy, u16 changed, u16 state) {
    if (changed & state & BUTTON_UP) {
        stateStart_buttonUp();
    } else if (changed & state & BUTTON_DOWN) {
        stateStart_buttonDown();
    } else if (changed & state & BUTTON_START) {
        stateStart_buttonStart();
    }
}

void stateStart_process() {
    stateStart_init();

    while (currentState == STATE_START) {
        stateStart_update();

        SPR_update();
        SPR_defragVRAM();
        SYS_doVBlankProcess();
    }

    // release all resources
    JOY_setEventHandler(NULL);
    stateStart_release();
    VDP_init();
}