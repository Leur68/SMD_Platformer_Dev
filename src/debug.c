#include "../inc/global.h"

#if (DEBUG_SLOW_MODE)
bool hasSlowModeEnabled = false;
#endif

void debug_print()
{
    static bool debugInited = false;
    static u16 len = 4;
    u16 i = 0;
    u16 x;

    if (!debugInited)
    {
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

#if (DEBUG_WINDOW_PLAYER_BOOLS)
        VDP_drawText("j", x, i++);
        VDP_drawText("f", x, i++);
        VDP_drawText("m", x, i++);
        VDP_drawText("am", x, i++);
        VDP_drawText("d", x, i++);
#endif

#if (DEBUG_WINDOW_PLAYER_INTS)
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
    drawInt(player->globalAABB.x.min, x, i++, len);
    drawInt(player->globalAABB.x.max, x, i++, len);
    drawInt(player->globalAABB.y.min, x, i++, len);
    drawInt(player->globalAABB.y.max, x, i++, len);
    drawInt(player->screenPos.x, x, i++, len);
    drawInt(player->screenPos.y, x, i++, len);
#endif

#if (DEBUG_WINDOW_PLAYER_MOVE)
    drawInt(player->velocity.x != FASTFIX32(0), x, i++, len);
    drawInt(player->velocity.y != FASTFIX32(0), x, i++, len);
    drawInt(player->movedPixels.x, x, i++, len);
    drawInt(player->movedPixels.y, x, i++, len);
#endif

#if (DEBUG_WINDOW_PLAYER_OBSTACLES)
    drawInt(player->inUpperObstacle, x, i++, len);
    drawInt(player->inLowerObstacle, x, i++, len);
    drawInt(player->inLeftObstacle, x, i++, len);
    drawInt(player->inRightObstacle, x, i++, len);
#endif

#if (DEBUG_WINDOW_PLAYER_BOOLS)
    drawInt(player->isJumping, x, i++, len);
    drawInt(player->isFalling, x, i++, len);
    drawInt(player->isMoving, x, i++, len);
    drawInt(player->isAutoMoving, x, i++, len);
    drawInt(player->decelerating, x, i++, len);
#endif

#if (DEBUG_WINDOW_PLAYER_INTS)
    drawInt(player->coyoteTimer, x, i++, len);
    drawInt(player->jumpTimer, x, i++, len);
    drawInt(player->facingDirection, x, i++, len);
#endif

#if (DEBUG_WINDOW_PLAYER_CAMERA)
    drawInt(cameraPosition.x, x, i++, len);
    drawInt(cameraPosition.y, x, i++, len);
    drawInt(backPosition.x, x, i++, len);
    drawInt(backPosition.y, x, i++, len);
#endif
}

void kdebug_print()
{
    static char str[500];
    static char tmp[16];
    str[0] = '\n';
    str[1] = '\0';

    char *p = str;

    while (*p)
        p++;

    char *s;

#define mystrcat()      \
    while (*p++ = *s++) \
        ;               \
    --p;

#define concU(t, v)       \
    s = t;                \
    mystrcat();           \
    uintToStr(v, tmp, 1); \
    s = tmp;              \
    mystrcat();           \
    s = "\n";             \
    mystrcat();

#define concS(t, v)      \
    s = t;               \
    mystrcat();          \
    intToStr(v, tmp, 1); \
    s = tmp;             \
    mystrcat();          \
    s = "\n";            \
    mystrcat();

#define conc_() \
    s = "\n";   \
    mystrcat();

    concU("x1  ", player->globalAABB.x.min)
    concU("x2  ", player->globalAABB.x.max)
    concU("y1  ", player->globalAABB.y.min)
    concU("y2  ", player->globalAABB.y.max)
    concU("sx  ", player->screenPos.x)
    concU("xy  ", player->screenPos.y)
    conc_()

    concU("vxb ", player->velocity.x != FASTFIX32(0))
    concU("vyb ", player->velocity.y != FASTFIX32(0))
    concS("mpX ", player->movedPixels.x)
    concS("mpY ", player->movedPixels.y)
    conc_()

    concU("u   ", player->inUpperObstacle)
    concU("g   ", player->inLowerObstacle)
    concU("l   ", player->inLeftObstacle)
    concU("r   ", player->inRightObstacle)
    conc_()

    concU("ju  ", player->isJumping)
    concU("fa  ", player->isFalling)
    concU("mo  ", player->isMoving)
    concU("amo ", player->isAutoMoving)
    concU("de  ", player->decelerating)
    conc_()

    concU("cot ", player->coyoteTimer)
    concU("jut ", player->jumpTimer)
    concU("fd  ", player->facingDirection)
    concU("s   ", scrolled)
    conc_()

    concU("cx  ", cameraPosition.x)
    concU("cy  ", cameraPosition.y)
    concU("bx  ", backPosition.x)
    concU("by  ", backPosition.y)

    KDebug_Alert(str);
}