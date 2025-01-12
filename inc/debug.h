#include <global.h>

#define RELEASE                            0

#define DEBUG_SHOW_FPS                     1
#define DEBUG_SHOW_CPU                     1

#define DEBUG_WINDOW                       0 // Боковая панель (WINDOW PLANE) с отладочной информацией
#define DEBUG_WINDOW_PLAYER_POS            0
#define DEBUG_WINDOW_PLAYER_MOVE           0
#define DEBUG_WINDOW_PLAYER_OBSTACLES      0
#define DEBUG_WINDOW_PLAYER_OTHER_BOOLS    1
#define DEBUG_WINDOW_PLAYER_OTHER_INTS     1
#define DEBUG_WINDOW_PLAYER_CAMERA         0

#define DEBUG_KDEBUG                       1 // Панель с отладочной информацией в консоли. За один фрейм выводятся все значения в одном принте

#define DEBUG_COLLISIONS                   0 // Отключение гравитации, свободное перемещение. Визуальная демонстрация тайлов, пересекающихся с персонажем. Работает нестабильно
#define DEBUG_INTERRUPT                    1 // При ошибке выполнение останавливается и выводится описание ошибки (требуется прописывать для каждого конкретного отлаживаемого фрагмента)
#define DISABLE_INTROS                     1 // Отключение всех states кроме game
#define SLOW_MODE                          1 // Доступность режима замедления. При нажатии на mode игра сильно замедляется

#if (SLOW_MODE)
    extern bool hasSlowModeEnabled;
#endif

#if (DEBUG_COLLISIONS)
    extern Sprite* collisionCursors[12];
    extern Sprite* playerCursor;
#endif

void debug_print();
void kdebug_print();