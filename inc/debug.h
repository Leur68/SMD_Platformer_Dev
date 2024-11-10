#include <global.h>

#define DEBUG_WINDOW                       1 // Боковая панель (WINDOW PLANE) с отладочной информацией
#define DEBUG_WINDOW_PLAYER_POS            1
#define DEBUG_WINDOW_PLAYER_MOVE           1
#define DEBUG_WINDOW_PLAYER_OBSTACLES      1
#define DEBUG_WINDOW_PLAYER_OTHER_BOOLS    0
#define DEBUG_WINDOW_PLAYER_OTHER_INTS     0
#define DEBUG_WINDOW_PLAYER_CAMERA         0

#define DEBUG_COLLISIONS       0 // Отключение гравитации, свободное перемещение. Визуальная демонстрация тайлов, пересекающихся с персонажем
#define DEBUG_INTERRUPT        1 // При ошибке выполнение останавливается и выводится описание ошибки
#define SLOW_MODE              1 // Доступность режима замедления. При нажатии на mode игра сильно замедляется

#if (SLOW_MODE)
    extern bool hasSlowModeEnabled;
#endif

#if (DEBUG_COLLISIONS)
    extern Sprite* tileCursorsR[12];
    extern Sprite* playerCursor;
#endif

void debug_drawInt(s32 num, u16 x, u16 y, u16 len);
void debug_drawFix(f32 num, u16 x, u16 y, u16 len);
void debug();