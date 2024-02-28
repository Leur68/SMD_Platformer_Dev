#include "../inc/game.h"

int main(resetType) {
    engine_disableSoftReset(resetType);
    states_setLevel1State();
	engine_init();
    
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
            case STATE_PLAY_1 : {
                stateLevel1_process();
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