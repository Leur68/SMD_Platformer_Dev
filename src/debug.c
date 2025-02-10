#include "../inc/global.h"

#if (DEBUG_SLOW_MODE)
bool hasSlowModeEnabled = false;
#endif

void debug_print() {
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
#endif

#if (DEBUG_WINDOW_PLAYER_MOVE)
        VDP_drawText("vxb", x, i++);
        VDP_drawText("vyb", x, i++);
        VDP_drawText("mX", x, i++);
        VDP_drawText("mY", x, i++);
#endif

#if (DEBUG_WINDOW_PLAYER_OBSTACLES)
        VDP_drawText("l", x, i++);
        VDP_drawText("r", x, i++);
        VDP_drawText("t", x, i++);
        VDP_drawText("b", x, i++);
#endif

#if (DEBUG_WINDOW_PLAYER_BOOLS)
        VDP_drawText("j", x, i++);
        VDP_drawText("f", x, i++);
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
    drawInt(player->collider->globalAABB.x.min, x, i++, len);
    drawInt(player->collider->globalAABB.x.max, x, i++, len);
    drawInt(player->collider->globalAABB.y.min, x, i++, len);
    drawInt(player->collider->globalAABB.y.max, x, i++, len);
#endif

#if (DEBUG_WINDOW_PLAYER_MOVE)
    drawInt(player->velocity.x != FASTFIX32(0), x, i++, len);
    drawInt(player->velocity.y != FASTFIX32(0), x, i++, len);
    drawInt(player->movedPixels.x, x, i++, len);
    drawInt(player->movedPixels.y, x, i++, len);
#endif

#if (DEBUG_WINDOW_PLAYER_OBSTACLES)
    drawInt(GET_LEFT_COLLISION(player->collider), x, i++, len);
    drawInt(GET_RIGHT_COLLISION(player->collider), x, i++, len);
    drawInt(GET_TOP_COLLISION(player->collider), x, i++, len);
    drawInt(GET_BOTTOM_COLLISION(player->collider), x, i++, len);
#endif

#if (DEBUG_WINDOW_PLAYER_BOOLS)
    drawInt(player->isJumping, x, i++, len);
    drawInt(player->isFalling, x, i++, len);
    drawInt(player->decelerating, x, i++, len);
#endif

#if (DEBUG_WINDOW_PLAYER_INTS)
    drawInt(player->coyoteTimer, x, i++, len);
    drawInt(player->jumpTimer, x, i++, len);
    drawInt(player->collider->facingDirection, x, i++, len);
#endif

#if (DEBUG_WINDOW_PLAYER_CAMERA)
    drawInt(cameraPosition.x, x, i++, len);
    drawInt(cameraPosition.y, x, i++, len);
    drawInt(backPosition.x, x, i++, len);
    drawInt(backPosition.y, x, i++, len);
#endif
}

void kdebug_print() {
    u8 left     = GET_LEFT_COLLISION(player->collider);
    u8 right    = GET_RIGHT_COLLISION(player->collider);
    u8 top      = GET_TOP_COLLISION(player->collider);
    u8 bottom   = GET_BOTTOM_COLLISION(player->collider);
    bool ground = HAS_GROUND_COLLISION(player->collider);
    bool anyCollision = HAS_ANY_COLLISION(player->collider);

    static char str[500];
    static char tmp[16];
    str[0] = '\n';
    str[1] = '\0';

    char *p = str;

    while (*p)
        p++;

    char *s;

#define mystrcat()        \
    while (*p++ = *s++);  \
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

#define concff32(t, v)   \
    s = t;               \
    mystrcat();          \
    if (v < FASTFIX32(0)) { \
        s = "-";             \
        mystrcat();          \
        intToStr(fastFix32ToInt(-v), tmp, 1); \
    } else { \
        intToStr(fastFix32ToInt(v), tmp, 1); \
    } \
    s = tmp;             \
    mystrcat();          \
    s = ".";             \
    mystrcat();          \
    intToStr(((fastFix32Frac(v) * 1000) / 65536), tmp, 1); \
    s = tmp;             \
    mystrcat();          \
    s = "\n";            \
    mystrcat();

#define conc_() \
    s = "\n";   \
    mystrcat();

    concU("x  ", player->collider->globalAABB.x.min)
    concU("xm  ", player->collider->globalAABB.x.max)
    concU("y  ", player->collider->globalAABB.y.min)
    concU("ym  ", player->collider->globalAABB.y.max)

    if (collidedObject != NULL) {
        concU("ox  ", (collidedObject->globalAABB.x.min))
        concU("oxm  ", (collidedObject->globalAABB.x.max))
        concU("oy  ", (collidedObject->globalAABB.y.min))
        concU("oym  ", (collidedObject->globalAABB.y.max))
    } else {
        concU("ox  ", 0)
        concU("oxm  ", 0)
        concU("oy  ", 0)
        concU("oym  ", 0)
    }
    conc_()

    concff32("vx ", player->velocity.x)
    concff32("vy ", player->velocity.y)
    concff32("avx ", player->autoVelocity.x)
    concff32("avy ", player->autoVelocity.y)
    concS("mX ", player->movedPixels.x)
    concS("mY ", player->movedPixels.y)
    conc_()

    concU("l ", left)
    concU("r ", right)
    concU("t ", top)
    concU("b ", bottom)
    concU("g ", ground)
    concU("a ", anyCollision)
    conc_()

    concU("j ", player->isJumping)
    concU("f ", player->isFalling)
    concU("de ", player->decelerating)
    conc_()

    concU("c ", player->coyoteTimer)
    concU("jt ", player->jumpTimer)
    concU("wt ", player->waterTimer)
    concU("d ", player->collider->facingDirection)
    concU("s ", scrolled)
    conc_()

    concU("cx ", cameraPosition.x)
    concU("cy ", cameraPosition.y)
    concU("bx ", backPosition.x)
    concU("by ", backPosition.y)

    KDebug_Alert(str);
}