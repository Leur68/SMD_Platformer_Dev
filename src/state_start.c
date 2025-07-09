#include "global.h"

u8 cursorIndex;
Sprite *cursor;

Option options[NUM_OPTIONS] = {
    {8, 8, "Start"}, {8, 9, "Options"},
};

void stateStart_init() {
    JOY_setEventHandler(&stateStart_joyHandler);

    VDP_drawText("Menu", 8, 5);

    cursor = SPR_addSpriteSafe(&cursor_spr, 0, 0, 0);
    cursorIndex = 0;
    stateStart_updateCursorPosition();

    for (u16 i = 0; i < NUM_OPTIONS; i++) {
        Option o = options[i];
        VDP_drawText(o.label, o.x, o.y);
    }

    // PAL_setColors(0, palette_grey, 64, DMA);
    // fadeInScreen(NORMAL_FADE);
}

void stateStart_buttonUp() {
    stateStart_setCursorIndexUp();
    stateStart_updateCursorPosition();
}

void stateStart_buttonDown() {
    stateStart_setCursorIndexDown();
    stateStart_updateCursorPosition();
}

void stateStart_buttonStart() {
    stateStart_select(cursorIndex);
}

void stateStart_update() {
}

void stateStart_release() {
    SPR_releaseSprite(cursor);
    cursor = NULL;
}

void stateStart_setCursorIndexUp() {
    if (cursorIndex > 0) {
        cursorIndex--;
    } else {
        cursorIndex = NUM_OPTIONS - 1;
    }
}

void stateStart_setCursorIndexDown() {
    if (cursorIndex < NUM_OPTIONS - 1) {
        cursorIndex++;
    } else {
        cursorIndex = 0;
    }
}

void stateStart_updateCursorPosition() {
    SPR_setPosition(cursor, options[cursorIndex].x * 8 - 12, options[cursorIndex].y * 8);
}

void stateStart_select(u16 option) {
    switch (option) {
    case 0: {
        // Start
        currentState = STATE_GAME;
        break;
    }
    case 1: {
        // Options
        currentState = STATE_OPTIONS;
        break;
    }
    default:
        break;
    }
}