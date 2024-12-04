#include "../inc/global.h"

#if (SLOW_MODE)
    bool hasSlowModeEnabled = false;
#endif

#if (DEBUG_COLLISIONS)
    Sprite* tileCursorsR[12] = {};
    Sprite* playerCursor;
#endif

void debug_drawInt(s32 num, u16 x, u16 y, u16 len) {
    char result[len + 1];
    intToStr(num, result, 1);
    VDP_drawTextFill(result, x, y, len);
}

void debug_drawFix(f32 num, u16 x, u16 y, u16 len) {
    char result[len + 1];
    fix32ToStr(num, result, 1);
    VDP_drawTextFill(result, x, y, len);
}

void debug() {
    static bool debugInited = false;
    static u16 len = 4;
    u16 i = 0; 
    u16 x;

    if (!debugInited) {
        VDP_setWindowHPos(true, 16);

        x = 32;

        #if (DEBUG_WINDOW_PLAYER_POS)
            VDP_drawText("x1", x, i++);
            VDP_drawText("x2", x, i++);
            VDP_drawText("y1", x, i++);
            VDP_drawText("y2", x, i++);
            VDP_drawText("sx", x, i++);
            VDP_drawText("xy", x, i++);
        #endif

        #if (DEBUG_WINDOW_PLAYER_MOVE)
            VDP_drawText("vxb", x, i++);
            VDP_drawText("vyb", x, i++);
            VDP_drawText("mX", x, i++);
            VDP_drawText("mY", x, i++);
        #endif

        #if (DEBUG_WINDOW_PLAYER_OBSTACLES)
            VDP_drawText("u", x, i++);
            VDP_drawText("g", x, i++);
            VDP_drawText("l", x, i++);
            VDP_drawText("r", x, i++);
        #endif

        #if (DEBUG_WINDOW_PLAYER_OTHER_BOOLS)
            VDP_drawText("j", x, i++);
            VDP_drawText("f", x, i++);
            VDP_drawText("m", x, i++);
            VDP_drawText("am", x, i++);
            VDP_drawText("d", x, i++);
        #endif

        #if (DEBUG_WINDOW_PLAYER_OTHER_INTS)
            VDP_drawText("ct", x, i++);
            VDP_drawText("jt", x, i++);
            VDP_drawText("f", x, i++);
        #endif

        #if (DEBUG_WINDOW_PLAYER_CAMERA)
            VDP_drawText("cx", x, i++);
            VDP_drawText("cy", x, i++);
            VDP_drawText("bx", x, i++);
            VDP_drawText("by", x, i++);

        #endif

        debugInited = true;
        i = 0; 
    }

    x = 36;

    #if (DEBUG_WINDOW_PLAYER_POS)
        debug_drawInt(player->globalAABB.x.min, x, i++, len);
        debug_drawInt(player->globalAABB.x.max, x, i++, len);
        debug_drawInt(player->globalAABB.y.min, x, i++, len);
        debug_drawInt(player->globalAABB.y.max, x, i++, len); 
        debug_drawInt(player->screenPos.x, x, i++, len);
        debug_drawInt(player->screenPos.y, x, i++, len);  
    #endif

    #if (DEBUG_WINDOW_PLAYER_MOVE)
        debug_drawInt(player->velocity.x != FASTFIX32(0), x, i++, len);
        debug_drawInt(player->velocity.y != FASTFIX32(0), x, i++, len);
        debug_drawInt(player->movedPixels.x, x, i++, len);
        debug_drawInt(player->movedPixels.y, x, i++, len);
    #endif

    #if (DEBUG_WINDOW_PLAYER_OBSTACLES)
        debug_drawInt(player->inUpperObstacle, x, i++, len);
        debug_drawInt(player->inLowerObstacle, x, i++, len);
        debug_drawInt(player->inLeftObstacle, x, i++, len);
        debug_drawInt(player->inRightObstacle, x, i++, len);
    #endif

    #if (DEBUG_WINDOW_PLAYER_OTHER_BOOLS)
        debug_drawInt(player->isJumping, x, i++, len);
        debug_drawInt(player->isFalling, x, i++, len);
        debug_drawInt(player->isMoving, x, i++, len);
        debug_drawInt(player->isAutoMoving, x, i++, len);
        debug_drawInt(player->decelerating, x, i++, len);
    #endif

    #if (DEBUG_WINDOW_PLAYER_OTHER_INTS)
        debug_drawInt(player->coyoteTimer, x, i++, len);
        debug_drawInt(player->jumpTimer, x, i++, len);
        debug_drawInt(player->facingDirection, x, i++, len);
    #endif

    #if (DEBUG_WINDOW_PLAYER_CAMERA)
        debug_drawInt(cameraPosition.x, x, i++, len);
        debug_drawInt(cameraPosition.y, x, i++, len);
        debug_drawInt(backPosition.x, x, i++, len);
        debug_drawInt(backPosition.y, x, i++, len);
    #endif
}

//char* mystrcat(char* dest, char* src) {
//    while (*dest) dest++;
//    while (*dest++ = *src++);
//    return --dest;
//}

void kdebug() {
    static char str[500];
    static char tmp[16];
    str[0] = '\n';
    str[1] = '\0';

    char* p = str;
    
    while (*p) p++;
    
    char* s;

    #define mystrcat() \
        while (*p++ = *s++); \
        --p;

    #define concU(t, v) \
        s = t; \
        mystrcat(); \
        uintToStr(v, tmp, 1); \
        s = tmp; \
        mystrcat(); \
        s = "\n"; \
        mystrcat();

    #define concS(t, v) \
        s = t; \
        mystrcat(); \
        intToStr(v, tmp, 1); \
        s = tmp; \
        mystrcat(); \
        s = "\n"; \
        mystrcat();

    concU("x1  ", player->globalAABB.x.min)
    concU("x2  ", player->globalAABB.x.max)
    concU("y1  ", player->globalAABB.y.min)
    concU("y2  ", player->globalAABB.y.max)
    concU("sx  ", player->screenPos.x)
    concU("xy  ", player->screenPos.y)

    concU("vxb ", player->velocity.x != FASTFIX32(0))
    concU("vyb ", player->velocity.y != FASTFIX32(0))
    concS("mX  ", player->movedPixels.x)
    concS("mY  ", player->movedPixels.y)

    concU("u   ", player->inUpperObstacle)
    concU("g   ", player->inLowerObstacle)
    concU("l   ", player->inLeftObstacle)
    concU("r   ", player->inRightObstacle)

    concU("j   ", player->isJumping)
    concU("f   ", player->isFalling)
    concU("m   ", player->isMoving)
    concU("am  ", player->isAutoMoving)
    concU("d   ", player->decelerating)

    concU("ct  ", player->coyoteTimer)
    concU("jt  ", player->jumpTimer)
    concU("f   ", player->facingDirection)

    concU("cx  ", cameraPosition.x)
    concU("cy  ", cameraPosition.y)
    concU("bx  ", backPosition.x)
    concU("by  ", backPosition.y)

    KDebug_Alert(str);
}