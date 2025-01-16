#include <global.h>

typedef struct {
    ff32 x;
    ff32 y;
} Vect2D_ff32;

typedef struct {
    u16 min;
    u16 max;
} AxisLine_u16;

s16 math_roundUpByEight(s16 x);
s16 math_roundDownByEight(s16 x);
bool math_isMultipleOfEight(int num);