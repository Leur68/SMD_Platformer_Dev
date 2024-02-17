#include "../inc/game.h"

GAME_STATE currentState;

void states_setIntroState() {
    currentState = STATE_INTRO;
}

void states_setStartState() {
    currentState = STATE_START;
}

void states_setLevel1State() {
    currentState = STATE_PLAY_1;
}

void states_setOptionsState() {
    currentState = STATE_OPTIONS;
}