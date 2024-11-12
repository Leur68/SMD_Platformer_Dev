#include "../inc/global.h"

int main(resetType) {
    disableSoftReset(resetType);
    if (DISABLE_INTROS) {
        states_setGameState();
    } else {
        states_setIntroState();
    }
	global_init();
    
    while(true) {
        switch(currentState) {
            case STATE_INTRO : {
                stateIntro_process();
                break;
            }
            case STATE_START : {
                stateStart_process();
                break;
            }
            case STATE_GAME : {
                stateGame_process();
                break;
            }
            case STATE_OPTIONS : {
                stateOptions_process();
                break;
            }
        }
    }
    
    return 0;
}