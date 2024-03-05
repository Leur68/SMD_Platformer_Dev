#include "../inc/game.h"

void engine_init() {
    VDP_setScreenWidth320();
	// JOY_init(); // Automatically called at SGDK initialization, no need to call it manually
	SPR_init();
}

// resetType это параметр главной функции игры
void engine_disableSoftReset(bool resetType) {
    if (!resetType)
        SYS_hardReset();
}

void engine_setBlackCRAM() {
    PAL_setColors(0, palette_black, 64, DMA);               // Устанавливаем чёрный экран
}

void engine_setGreyCRAM() {
    PAL_setColors(0, palette_grey, 64, DMA);                // Устанавливаем серую палитру экран
}

void engine_fadeInImage(Image img, u16 numFrame) {
    PAL_setColors(0, palette_black, 64, DMA);               // Устанавливаем чёрный экран
    PAL_fadeIn(0, 63, img.palette->data, numFrame, true);   // Производим эффект FadeIn для нужной палитры
}

void engine_fadeInPalette(const u16* pal, u16 numFrame) {
    PAL_setColors(0, palette_black, 64, DMA);               // Устанавливаем чёрный экран
    PAL_fadeIn(0, 63, pal, numFrame, true);                 // Производим эффект FadeIn для нужной палитры
}

void engine_fadeInScreen(u16 numFrame) {
    u16 palette_full[64];                                   // Буфер для хранения содержимого CRAM
    PAL_getColors(0, palette_full, 64);                     // Копируем в буфер содержимое CRAM (перед следующим вызовом, т.к. иначе скопировать не получится)
    PAL_setColors(0, palette_black, 64, DMA);               // Устанавливаем чёрный экран
    PAL_fadeIn(0, 63, palette_full, numFrame, true);        // Производим эффект FadeIn для нужной палитры
}

void engine_fadeOutScreen(u16 numFrame) {
    PAL_fadeOut(0, 63, numFrame, false);                    // Производим эффект FadeOut для всего экрана. async = false, чтобы не происходил преждевременный переход к следующей сцене
}

void engine_drawInt(const char* text, s16 num, u16 x, u16 y) {
    VDP_clearText(x + 4, y, 6);
    
    char numStr[6];
    intToStr(num, &numStr, 1);

    char result[10];

    strcpy(result, text);
    strcat(result, " ");
    for (u8 i = 0; i < 3 - strlen(text); i++) {
        strcat(result, " ");
    }
    strcat(result, numStr);

    VDP_drawText(result, x, y);
}

void engine_drawFix32(const char* text, f32 num, u16 x, u16 y) {
    VDP_clearText(x + 4, y, 1);
    
    char numStr[6];
    fix32ToStr(num, &numStr, 6);

    char result[10];

    strcpy(result, text);
    strcat(result, " ");
    for (u8 i = 0; i < 3 - strlen(text); i++) {
        strcat(result, " ");
    }
    strcat(result, numStr);

    VDP_drawText(result, x, y);
}

bool engine_isTileSolid(u8* collisions, s16 xTile, s16 yTile, u16 mapWTiles, u16 mapHTiles) {
    // Проверяем границы карты
    if (xTile < 0 || yTile < 0 || xTile >= mapWTiles || yTile >= mapHTiles) return true;
    // Возвращаем состояние тайла
    return masPointer2(collisions, yTile, xTile, mapWTiles) != COL_NONE;
}

AABB engine_checkMapArea(u8* collisions, AABB aabb, u16 mapWTiles, u16 mapHTiles) {
    AABB res;
    res.x.min = 0;
    res.y.min = 0;
    res.x.max = 0;
    res.y.max = 0;
    res.xTiles.min = 0;
    res.yTiles.min = 0;
    res.xTiles.max = 0;
    res.yTiles.max = 0;
    res.exists = false;
    bool start = true;
    for (s16 currYTile = aabb.yTiles.min; currYTile < aabb.yTiles.max; currYTile++) {
        for (s16 currXTile = aabb.xTiles.min; currXTile < aabb.xTiles.max; currXTile++) {
            if (engine_isTileSolid(collisions, currXTile, currYTile, mapWTiles, mapHTiles)) {
                if (start) {
                    res.xTiles.min = currXTile;
                    res.yTiles.min = currYTile;
                    res.x.min = res.xTiles.min << 3;
                    res.y.min = res.yTiles.min << 3;
                }
                res.xTiles.max = currXTile;
                res.yTiles.max = currYTile;
                res.x.max = res.xTiles.max << 3;
                res.y.max = res.yTiles.max << 3;
                // Если AABB шириной в один тайл, без корректировок ниже он будет шириной в 0 пикселей, что неверно
                if (res.y.max == res.y.min) {
                    res.y.max += 8;
                }
                if (res.x.max == res.x.min) {
                    res.x.max += 8;
                }
                start = false;
                res.exists = true;
            }
        }
    }
    return res;
}

bool engine_isOverlappingAxisLines(AxisLine_s16 line1, AxisLine_s16 line2) {
    return line1.max >= line2.min && line2.max >= line1.min;
}

bool engine_isOverlappingAABBs(AABB aabb1, AABB aabb2) {
    return (aabb1.x.max >= aabb2.x.min && aabb2.x.max >= aabb1.x.min) && (aabb1.y.max >= aabb2.y.min && aabb2.y.max >= aabb1.y.min);
}

void engine_shiftAABB(AABB* aabb, s8 x, s8 y) {
    aabb->x.min = aabb->x.min + x;
    aabb->x.max = aabb->x.max + x;
    aabb->y.min = aabb->y.min + y;
    aabb->y.max = aabb->y.max + y;
    engine_initAABBTileIndexes(aabb);
}

void engine_setAABB(AABB* aabb, s16 x, s16 y) {
    aabb->x.min = x;
    aabb->x.max = x + PLAYER_WIDTH;
    aabb->y.min = y;
    aabb->y.max = y + PLAYER_HEIGHT;
    engine_initAABBTileIndexes(aabb);
}

AABB engine_getTopAABB(AABB aabb) {
    AABB aabbTop;
    aabbTop.x = aabb.x;
    aabbTop.xTiles = aabb.xTiles;

    aabbTop.y.min = aabb.y.min - 8;
    aabbTop.y.max = aabb.y.min;

    aabbTop.yTiles.min = aabbTop.y.min >> 3;
    aabbTop.yTiles.max = aabb.yTiles.min;

    if (!engine_isMultipleOfEight(aabbTop.y.min)) {
        aabbTop.yTiles.min += 1;
    }
    if (!engine_isMultipleOfEight(aabbTop.y.max)) {
        aabbTop.yTiles.max += 1;
    }
    if (!engine_isMultipleOfEight(aabbTop.x.max)) {
        aabbTop.xTiles.max += 1;
    }

    return aabbTop;
}

AABB engine_getBottomAABB(AABB aabb) {
    AABB aabbBottom;
    aabbBottom.x = aabb.x;
    aabbBottom.xTiles = aabb.xTiles;

    aabbBottom.y.min = aabb.y.max;
    aabbBottom.y.max = aabb.y.max + 8;
    
    aabbBottom.yTiles.min = aabb.yTiles.max;
    aabbBottom.yTiles.max = aabbBottom.y.max >> 3;

    if (!engine_isMultipleOfEight(aabbBottom.x.max)) {
        aabbBottom.xTiles.max += 1;
    }

    return aabbBottom;
}

AABB engine_getLeftAABB(AABB aabb) {
    AABB aabbLeft;
    aabbLeft.y = aabb.y;
    aabbLeft.yTiles = aabb.yTiles;

    aabbLeft.x.min = aabb.x.min - 8;
    aabbLeft.x.max = aabb.x.min;
    
    aabbLeft.xTiles.min = aabbLeft.x.min >> 3;
    aabbLeft.xTiles.max = aabb.xTiles.min;

    if (!engine_isMultipleOfEight(aabbLeft.x.min)) {
        aabbLeft.xTiles.min += 1;
    }
    if (!engine_isMultipleOfEight(aabbLeft.x.max)) {
        aabbLeft.xTiles.max += 1;
    }
    if (!engine_isMultipleOfEight(aabbLeft.y.max)) {
        aabbLeft.yTiles.max += 1;
    }

    return aabbLeft;
}

AABB engine_getRightAABB(AABB aabb) {
    AABB aabbRight;
    aabbRight.y = aabb.y;
    aabbRight.yTiles = aabb.yTiles;

    aabbRight.x.min = aabb.x.max;
    aabbRight.x.max = aabb.x.max + 8;
    
    aabbRight.xTiles.min = aabb.xTiles.max;
    aabbRight.xTiles.max = aabbRight.x.max >> 3;

    if (!engine_isMultipleOfEight(aabbRight.y.max)) {
        aabbRight.yTiles.max += 1;
    }

    return aabbRight;
}

void engine_initAABBTileIndexes(AABB* aabb) {
    aabb->xTiles.min = aabb->x.min >> 3;
    aabb->xTiles.max = aabb->x.max >> 3;
    aabb->yTiles.min = aabb->y.min >> 3;
    aabb->yTiles.max = aabb->y.max >> 3;
}

s16 engine_roundUpByEight(s16 x) {
    return (x + 7) & (-8);
}

s16 engine_roundDownByEight(s16 x) {
    return x & (-8);
}

bool engine_isMultipleOfEight(int num) {
    return (num & 7) == 0;
}

#if (DEBUG_COLLISIONS)
    Sprite* tileCursorsR[12] = {};
    Sprite* playerCursor;
#endif

void engine_checkCollisions(AABB aabb, u8* collisionsMap, u16 mapWTiles, u16 mapHTiles, Vect2D_s8 direction, u8* left, u8* right, u8* top, u8* bottom) {
    // Получаем AABB для проверки столкновений
    AABB aabbLeft   = engine_getLeftAABB(aabb);
    AABB aabbRight  = engine_getRightAABB(aabb);
    AABB aabbTop    = engine_getTopAABB(aabb);
    AABB aabbBottom = engine_getBottomAABB(aabb);

    // Отсеиваем среди найденных тайлов только те, что являются твердыми на карте. Данными тайлами считаются те, вплотную к которым или внутри которых находятся соответствующие стороны AABB персонажа
    AABB aabbLeftObstacle   = engine_checkMapArea(collisionsMap, aabbLeft, mapWTiles, mapHTiles);
    AABB aabbRightObstacle  = engine_checkMapArea(collisionsMap, aabbRight, mapWTiles, mapHTiles);
    AABB aabbTopObstacle    = engine_checkMapArea(collisionsMap, aabbTop, mapWTiles, mapHTiles);
    AABB aabbBottomObstacle = engine_checkMapArea(collisionsMap, aabbBottom, mapWTiles, mapHTiles);

    // Рисуем тайлы с пересечениями для дебага
    #if (DEBUG_COLLISIONS)
        u8 tilesIR = 0;
        for (s16 x = aabbLeftObstacle.x.min; x < aabbLeftObstacle.x.max; x += 8) {    
            for (s16 y = aabbLeftObstacle.y.min; y < aabbLeftObstacle.y.max; y += 8) {
                SPR_setPosition(tileCursorsR[tilesIR++], x - mapShiftX, y - mapShiftY);
            }
        }
        for (u8 i = tilesIR; i < 3; i++) {
            SPR_setPosition(tileCursorsR[i], -8, -8);
        }
        tilesIR = 3;
        for (s16 x = aabbRightObstacle.x.min; x < aabbRightObstacle.x.max; x += 8) {    
            for (s16 y = aabbRightObstacle.y.min; y < aabbRightObstacle.y.max; y += 8) {
                SPR_setPosition(tileCursorsR[tilesIR++], x - mapShiftX, y - mapShiftY);
            }
        }
        for (u8 i = tilesIR; i < 6; i++) {
            SPR_setPosition(tileCursorsR[i], -8, -8);
        }
        tilesIR = 6;
        for (s16 x = aabbTopObstacle.x.min; x < aabbTopObstacle.x.max; x += 8) {    
            for (s16 y = aabbTopObstacle.y.min; y < aabbTopObstacle.y.max; y += 8) {
                SPR_setPosition(tileCursorsR[tilesIR++], x - mapShiftX, y - mapShiftY);
            }
        }
        for (u8 i = tilesIR; i < 9; i++) {
            SPR_setPosition(tileCursorsR[i], -8, -8);
        }
        tilesIR = 9;
        for (s16 x = aabbBottomObstacle.x.min; x < aabbBottomObstacle.x.max; x += 8) {    
            for (s16 y = aabbBottomObstacle.y.min; y < aabbBottomObstacle.y.max; y += 8) {
                SPR_setPosition(tileCursorsR[tilesIR++], x - mapShiftX, y - mapShiftY);
            }
        }
        for (u8 i = tilesIR; i < 12; i++) {
            SPR_setPosition(tileCursorsR[i], -8, -8);
        }
    #endif

    // Вычисляем длины пересечения
    *left   = 0;
    *right  = 0;
    *top    = 0;
    *bottom = 0;

    // Двигаемся только влево
    if (direction.x == DIRECTION_LEFT && direction.y == DIRECTION_NONE) {
        if (aabbLeftObstacle.exists) {
            *left = aabbLeftObstacle.x.max - aabb.x.min + 1;
        }
    } else 
    // Двигаемся только вправо
    if (direction.x == DIRECTION_RIGHT && direction.y == DIRECTION_NONE) {
        if (aabbRightObstacle.exists) {
            *right = aabb.x.max - aabbRightObstacle.x.min + 1;
        }
    } else 
    // Двигаемся только вверх
    if (direction.x == DIRECTION_NONE && direction.y == DIRECTION_UP) {
        if (aabbTopObstacle.exists) {
            *top = aabbTopObstacle.y.max - aabb.y.min + 1;
        }
    } else 
    // Двигаемся только вниз
    if (direction.x == DIRECTION_NONE && direction.y == DIRECTION_DOWN) {
        if (aabbBottomObstacle.exists) {
            *bottom = aabb.y.max - aabbBottomObstacle.y.min + 1;
        }
    } else
    // Двигаемся влево вверх
    if (direction.x == DIRECTION_LEFT && direction.y == DIRECTION_UP) {
        if (aabbLeftObstacle.exists) {
            s16 h = engine_getIntersectionLen(aabbLeftObstacle.x, aabb.x) + 1;
            s16 v = engine_getIntersectionLen(aabbLeftObstacle.y, aabb.y) + 1;

            if (v > h) {
                *left = h;
            } else if (v == h) {
                *left = h;
                *top = v;
            }
        }
        if (aabbTopObstacle.exists) {
            s16 h = engine_getIntersectionLen(aabbTopObstacle.x, aabb.x) + 1;
            s16 v = engine_getIntersectionLen(aabbTopObstacle.y, aabb.y) + 1;

            if (h > v) {
                *top = v;
            } else if (v == h) {
                *left = h;
                *top = v;
            }
        }
    } else
    // Двигаемся вправо вверх
    if (direction.x == DIRECTION_RIGHT && direction.y == DIRECTION_UP) {
        if (aabbRightObstacle.exists) {
            s16 h = engine_getIntersectionLen(aabbRightObstacle.x, aabb.x) + 1;
            s16 v = engine_getIntersectionLen(aabbRightObstacle.y, aabb.y) + 1;

            if (v > h) {
                *right = h;
            } if (v == h) {
                *right = h;
                *top = v;
            }
        }
        if (aabbTopObstacle.exists) {
            s16 h = engine_getIntersectionLen(aabbTopObstacle.x, aabb.x) + 1;
            s16 v = engine_getIntersectionLen(aabbTopObstacle.y, aabb.y) + 1;

            if (h > v) {
                *top = v;
            } if (v == h) {
                *right = h;
                *top = v;
            }
        }
    } else 
    // Двигаемся влево вниз
    if (direction.x == DIRECTION_LEFT && direction.y == DIRECTION_DOWN) {
        if (aabbLeftObstacle.exists) {
            s16 h = engine_getIntersectionLen(aabbLeftObstacle.x, aabb.x) + 1;
            s16 v = engine_getIntersectionLen(aabbLeftObstacle.y, aabb.y) + 1;
        
            if (v > h) {
                *left = h;
            } else if (v == h) {
                *left = h;
                *bottom = v;
            }
        }
        if (aabbBottomObstacle.exists) {
            s16 h = engine_getIntersectionLen(aabbBottomObstacle.x, aabb.x) + 1;
            s16 v = engine_getIntersectionLen(aabbBottomObstacle.y, aabb.y) + 1;
        
            if (h > v) {
                *bottom = v;
            } else if (v == h) {
                *left = h;
                *bottom = v;
            }
        }
    } else 
    // Двигаемся вправо вниз
    if (direction.x == DIRECTION_RIGHT && direction.y == DIRECTION_DOWN) {
        if (aabbRightObstacle.exists) {
            s16 h = engine_getIntersectionLen(aabbRightObstacle.x, aabb.x) + 1;
            s16 v = engine_getIntersectionLen(aabbRightObstacle.y, aabb.y) + 1;

            if (v > h) {
                *right = h;
            } else if (v == h) {
                *right = h;
                *bottom = v;
            }
        }
        if (aabbBottomObstacle.exists) {
            s16 h = engine_getIntersectionLen(aabbBottomObstacle.x, aabb.x) + 1;
            s16 v = engine_getIntersectionLen(aabbBottomObstacle.y, aabb.y) + 1;

            if (h > v) {
                *bottom = v;
            } else if (v == h) {
                *right = h;
                *bottom = v;
            }
        }
    }
}

s16 engine_getIntersectionLen(AxisLine_s16 a, AxisLine_s16 b) {
    s16 start = a.min > b.min ? a.min : b.min; // Большее из минимальных значений
    s16 end = a.max < b.max ? a.max : b.max;   // Меньшее из максимальных значений

    s16 len = end - start; // Вычисляем длину пересечения
    return len > 0 ? len : 0; // Если длина пересечения отрицательная, значит, пересечения нет
}