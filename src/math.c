#include "../inc/global.h"

s16 math_roundUpByEight(s16 x) {
    return (x + 7) & (-8);
}

s16 math_roundDownByEight(s16 x) {
    return x & (-8);
}

bool math_isMultipleOfEight(int num) {
    return (num & 7) == 0;
}