#include "../inc/global.h"

int main(resetType)
{
    disableSoftReset(resetType);
    if (DEBUG_DISABLE_INTROS)
    {
        currentState = STATE_GAME;
    }
    else
    {
        currentState = STATE_INTRO;
    }
    global_init();

    while (true)
    {
        switch (currentState)
        {
        case STATE_INTRO:
        {
            stateIntro_process();
            break;
        }
        case STATE_START:
        {
            stateStart_process();
            break;
        }
        case STATE_GAME:
        {
            stateGame_process();
            break;
        }
        case STATE_OPTIONS:
        {
            stateOptions_process();
            break;
        }
        }
    }

    return 0;
}