#include <genesis.h>

typedef enum {
    STATE_INTRO,   // Ряд заставок, отображаемых при запуске игры
    STATE_START,   // Экран запуска игры и настроек
    STATE_PLAY_1,  // Игра
    STATE_OPTIONS, // Настройки
    STATE_CREDITS, // Титры
} GAME_STATE;

extern GAME_STATE currentState;

void states_setIntroState();
void states_setStartState();
void states_setLevel1State();
void states_setOptionsState();