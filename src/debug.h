#include <global.h>

#define RELEASE                            0

#define DEBUG_SHOW_FPS                     1 // Show FPS on screen
#define DEBUG_SHOW_CPU                     1 // Show CPU load on screen
#define DEBUG_SHOW_FREE_RAM                1 // Show CPU load on screen

#define DEBUG_WINDOW                       0 // Sidebar (WINDOW PLANE) with debug information
#define DEBUG_WINDOW_PLAYER_POS            0 // Display player position in the debug window
#define DEBUG_WINDOW_PLAYER_MOVE           0 // Display player movement data in the debug window
#define DEBUG_WINDOW_PLAYER_OBSTACLES      0 // Display player collision information in the debug window
#define DEBUG_WINDOW_PLAYER_CAMERA         0 // Display camera data in the debug window
#define DEBUG_WINDOW_PLAYER_BOOLS          0 // Display player boolean flags in the debug window
#define DEBUG_WINDOW_PLAYER_INTS           0 // Display player integer variables in the debug window

#define DEBUG_KDEBUG_STATE                 0 // Debug information output in the console (KDebug)
#define DEBUG_KDEBUG_LOG                   0 // Debug information output in the console (KDebug)

#define DEBUG_INTERRUPT                    1 // On error, execution stops and an error description is printed (needs to be manually implemented for each debugged fragment)
#define DEBUG_DISABLE_INTROS               1 // Disable all states except the game state
#define DEBUG_SLOW_MODE                    1 // Enable slow-motion mode. When the mode button is pressed, the game slows down significantly
#define DEBUG_FREE_MOVE_MODE               0 // Disable gravity, collisions, enable free movement

#if (DEBUG_SLOW_MODE)
extern bool hasSlowModeEnabled;
#endif

void debug_printStateOnScreen();
void kdebug_printState();

#define mystrcat()        \
    while (*p++ = *s++);  \
    --p;

/*
    You should copy this:

    //////////////////////
    static char str[500];
    static char tmp[16];
    str[0] = '\n';
    str[1] = '\0';
    char *p = str;
    while (*p)
        p++;
    char *s;
    //////////////////////

    Then you should use functions at the bottom like this:

    concS("dx ", deltaX)
    concS("dy ", deltaY)
    conc_()
    concS("mX ", player->movedPixels.x)
    concS("mY ", player->movedPixels.y)

    At the end call following code:

    KDebug_Alert(str);
*/

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
        intToStr(FF32_toInt(-v), tmp, 1); \
    } else { \
        intToStr(FF32_toInt(v), tmp, 1); \
    } \
    s = tmp;             \
    mystrcat();          \
    s = ".";             \
    mystrcat();          \
    intToStr(((FF32_frac(v) * 1000) / 65536), tmp, 1); \
    s = tmp;             \
    mystrcat();          \
    s = "\n";            \
    mystrcat();

#define conc_() \
    s = "\n";   \
    mystrcat();