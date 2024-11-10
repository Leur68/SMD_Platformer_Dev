#include "../inc/global.h"

GAME_STATE currentState;

void states_setIntroState() {
    currentState = STATE_INTRO;
}

void states_setStartState() {
    currentState = STATE_START;
}

void states_setGameState() {
    currentState = STATE_GAME;
}

void states_setOptionsState() {
    currentState = STATE_OPTIONS;
}