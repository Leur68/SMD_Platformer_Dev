#include "../inc/global.h"

#if (SLOW_MODE)
    bool hasSlowModeEnabled = false;
#endif

#if (DEBUG_COLLISIONS)
    Sprite* tileCursorsR[12] = {};
    Sprite* playerCursor;
#endif

void engine_drawDebugInt(s32 num, u16 x, u16 y, u16 len) {
    char result[len + 1];
    intToStr(num, result, 1);
    VDP_drawTextFill(result, x, y, len);
}

void engine_drawDebugFix(f32 num, u16 x, u16 y, u16 len) {
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
        engine_drawDebugInt(player->globalAABB.x.min, x, i++, len);
        engine_drawDebugInt(player->globalAABB.x.max, x, i++, len);
        engine_drawDebugInt(player->globalAABB.y.min, x, i++, len);
        engine_drawDebugInt(player->globalAABB.y.max, x, i++, len); 
        engine_drawDebugInt(player->screenPos.x, x, i++, len);
        engine_drawDebugInt(player->screenPos.y, x, i++, len);  
    #endif

    #if (DEBUG_WINDOW_PLAYER_MOVE)
        engine_drawDebugInt(player->velocity.x != FASTFIX32(0), x, i++, len);
        engine_drawDebugInt(player->velocity.y != FASTFIX32(0), x, i++, len);
        engine_drawDebugInt(player->movedPixels.x, x, i++, len);
        engine_drawDebugInt(player->movedPixels.y, x, i++, len);
    #endif

    #if (DEBUG_WINDOW_PLAYER_OBSTACLES)
        engine_drawDebugInt(player->inUpperObstacle, x, i++, len);
        engine_drawDebugInt(player->inLowerObstacle, x, i++, len);
        engine_drawDebugInt(player->inLeftObstacle, x, i++, len);
        engine_drawDebugInt(player->inRightObstacle, x, i++, len);
    #endif

    #if (DEBUG_WINDOW_PLAYER_OTHER_BOOLS)
        engine_drawDebugInt(player->isJumping, x, i++, len);
        engine_drawDebugInt(player->isFalling, x, i++, len);
        engine_drawDebugInt(player->isMoving, x, i++, len);
    #endif

    #if (DEBUG_WINDOW_PLAYER_OTHER_INTS)
        engine_drawDebugInt(player->coyoteTimer, x, i++, len);
        engine_drawDebugInt(player->jumpTimer, x, i++, len);
        engine_drawDebugInt(player->facingDirection, x, i++, len);
    #endif

    #if (DEBUG_WINDOW_PLAYER_CAMERA)
        engine_drawDebugInt(cameraPosition.x, x, i++, len);
        engine_drawDebugInt(cameraPosition.y, x, i++, len);
        engine_drawDebugInt(backPosition.x, x, i++, len);
        engine_drawDebugInt(backPosition.y, x, i++, len);
    #endif
}