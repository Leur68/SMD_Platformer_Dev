#include "../inc/global.h"

u8 counter = 0;

void stateIntro_buttonStart()
{
    counter = 180;
}

void stateIntro_init()
{
    JOY_setEventHandler(&stateIntro_joyHandler);
    VDP_drawImage(BG_A, &sgdk, 15, 9);
}

void stateIntro_release()
{
}

void stateIntro_update()
{
    fadeInScreen(NORMAL_FADE);
    while (true)
    {
        SYS_doVBlankProcess();
        counter++;
        if (counter >= 180)
        {
            break;
        }
    }
    fadeOutScreen(NORMAL_FADE);
    currentState = STATE_START;
}