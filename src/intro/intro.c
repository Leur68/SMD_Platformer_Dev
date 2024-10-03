#include "../inc/game.h"

u8 counter = 0;

void stateIntro_buttonStart() {
    counter = 180;
}

void stateIntro_init() {
    JOY_setEventHandler(&stateIntro_joyHandler);
    VDP_drawImage(BG_A, &sgdk, 15, 9);
}

void stateIntro_update() {
    engine_fadeInScreen(NORMAL_FADE);
    while (true) {
        SYS_doVBlankProcess();
        counter++;
        if (counter >= 180) {
            break;
        }
    }
    engine_fadeOutScreen(NORMAL_FADE);
    states_setStartState();
}

void stateIntro_release() {
    JOY_setEventHandler(NULL);
    VDP_init();
}