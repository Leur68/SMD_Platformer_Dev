#include "../inc/game.h"

u8 counter = 0;

void stateIntro_buttonStart() {
    counter = 180;
}

void stateIntro_init() {
    JOY_setEventHandler(&stateIntro_joyHandler);
}

void stateIntro_update() {
    VDP_drawImage(BG_A, &sgdk, 15, 9);
    engine_fadeInScreen(NORMAL_FADE);
    while (TRUE) {
        SYS_doVBlankProcess();
        counter++;
        if (counter >= 180) {
            break;
        }
    }
    engine_fadeOutScreen(NORMAL_FADE);
    VDP_clearPlane(BG_A, TRUE);
    states_setStartState();
}

void stateIntro_release() {
    JOY_setEventHandler(NULL);
}