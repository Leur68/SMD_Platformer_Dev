#include <genesis.h>

#define NUM_OPTIONS 2
typedef struct {
    u16 x;
    u16 y;
    char label[30];
} Option;

void stateStart_init();
void stateStart_joyHandler(u16 joy, u16 changed, u16 state);
void stateStart_buttonStart();
void stateStart_buttonUp();
void stateStart_buttonDown();
void stateStart_process();
void stateStart_update();
void stateStart_updateCursorPosition();
void stateStart_setCursorIndexUp();
void stateStart_setCursorIndexDown();
void stateStart_select(u16 option);