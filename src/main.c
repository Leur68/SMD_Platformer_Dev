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















/*
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





#include <genesis.h>


extern void hello_world_asm();

// Основная программа
int main()
{
    // Инициализация экрана
    SPR_init();
    VDP_setScreenWidth320();
    //VDP_setTextPlane(BG_A);               // Устанавливаем текстовый план на план A
    //VDP_setTextPalette(0);                // Устанавливаем цветовую палитру для текста

    // Вызов ассемблерной функции
    hello_world_asm();

    // Ждем, пока не будет нажата кнопка
    while(TRUE)
    {
        SYS_doVBlankProcess();
    }

    return 0;
}
*/