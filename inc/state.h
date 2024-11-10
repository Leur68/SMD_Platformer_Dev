#include <global.h>

typedef enum {
    STATE_INTRO,   // Ряд заставок, отображаемых при запуске игры
    STATE_START,   // Экран запуска игры и настроек
    STATE_GAME,  // Игра
    STATE_OPTIONS, // Настройки
    STATE_CREDITS, // Титры
} GAME_STATE;

extern GAME_STATE currentState;

void states_setIntroState();
void states_setStartState();
void states_setGameState();
void states_setOptionsState();