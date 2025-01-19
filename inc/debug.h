#include <global.h>

#define RELEASE                            0

#define DEBUG_SHOW_FPS                     1
#define DEBUG_SHOW_CPU                     1

#define DEBUG_WINDOW                       0 // Боковая панель (WINDOW PLANE) с отладочной информацией
#define DEBUG_WINDOW_PLAYER_POS            0 
#define DEBUG_WINDOW_PLAYER_MOVE           0
#define DEBUG_WINDOW_PLAYER_OBSTACLES      0
#define DEBUG_WINDOW_PLAYER_CAMERA         0
#define DEBUG_WINDOW_PLAYER_BOOLS          1
#define DEBUG_WINDOW_PLAYER_INTS           1

#define DEBUG_KDEBUG                       1 // Панель с отладочной информацией в консоли

#define DEBUG_INTERRUPT                    1 // При ошибке выполнение останавливается и выводится описание ошибки (требуется прописывать для каждого конкретного отлаживаемого фрагмента)
#define DEBUG_DISABLE_INTROS               1 // Отключение всех states кроме game
#define DEBUG_SLOW_MODE                    1 // Доступность режима замедления. При нажатии на mode игра сильно замедляется
#define DEBUG_FREE_MOVE_MODE               0 // Отключение гравитации, свободное перемещение

#if (DEBUG_SLOW_MODE)
    extern bool hasSlowModeEnabled;
#endif

void debug_print();
void kdebug_print();