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
    PAL_fadeIn(0, 63, img.palette->data, numFrame, TRUE);   // Производим эффект FadeIn для нужной палитры
}

void engine_fadeInPalette(const u16 * pal, u16 numFrame) {
    PAL_setColors(0, palette_black, 64, DMA);               // Устанавливаем чёрный экран
    PAL_fadeIn(0, 63, pal, numFrame, TRUE);                 // Производим эффект FadeIn для нужной палитры
}

void engine_fadeInScreen(u16 numFrame) {
    u16 palette_full[64];                                   // Буфер для хранения содержимого CRAM
    PAL_getColors(0, palette_full, 64);                     // Копируем в буфер содержимое CRAM (перед следующим вызовом, т.к. иначе скопировать не получится)
    PAL_setColors(0, palette_black, 64, DMA);               // Устанавливаем чёрный экран
    PAL_fadeIn(0, 63, palette_full, numFrame, TRUE);        // Производим эффект FadeIn для нужной палитры
}

void engine_fadeOutScreen(u16 numFrame) {
    PAL_fadeOut(0, 63, numFrame, FALSE);                    // Производим эффект FadeOut для всего экрана. async = FALSE, чтобы не происходил преждевременный переход к следующей сцене
}

void engine_drawInt(const char* text, s16 num, u16 x, u16 y) {
    VDP_clearText(x + 4, y, 6);

    char numStr[6];
    intToStr(num, &numStr, 1);

    char result[10];

    strcpy(result, text);
    strcat(result, " ");
    for (u8 i = 0; i < 2 - strlen(text); i++) {
        strcat(result, " ");
    }
    strcat(result, numStr);

    VDP_drawText(result, x, y);
}

bool engine_isTileSolid(u8* collisions, s16 xTile, s16 yTile, u16 mapWTiles, u16 mapHTiles) {
    // Проверяем границы карты
    if (xTile < 0 || yTile < 0 || xTile >= mapWTiles || yTile >= mapHTiles) return TRUE;
    // Возвращаем состояние тайла
    return *(collisions + (yTile * mapWTiles) + xTile) == 0;
}

AABB engine_checkMapArea(u8* collisions, AABB aabb, u16 mapWTiles, u16 mapHTiles) {    
    AABB res;
    res.x.min = FIX32(0);
    res.y.min = FIX32(0);
    res.x.max = FIX32(0);
    res.y.max = FIX32(0);
    res.xTiles.min = FIX32(0);
    res.yTiles.min = FIX32(0);
    res.xTiles.max = FIX32(0);
    res.yTiles.max = FIX32(0);

    bool start = TRUE;
    for (s16 currYTile = aabb.yTiles.min; currYTile < aabb.yTiles.max; currYTile++) {
        for (s16 currXTile = aabb.xTiles.min; currXTile < aabb.xTiles.max; currXTile++) {
            if (engine_isTileSolid(collisions, currXTile, currYTile, mapWTiles, mapHTiles)) {
                if (start) {
                    res.xTiles.min = currXTile;
                    res.yTiles.min = currYTile;
                    res.x.min = intToFix32(res.xTiles.min << 3);
                    res.y.min = intToFix32(res.yTiles.min << 3);
                }
                res.xTiles.max = currXTile;
                res.yTiles.max = currYTile;
                res.x.max = intToFix32(res.xTiles.max << 3);
                res.y.max = intToFix32(res.yTiles.max << 3);
                // Если AABB шириной в один тайл, без корректировок ниже он будет шириной в 0 пикселей, что неверно
                if (res.y.max == res.y.min) {
                    res.y.max += FIX32(8);
                }
                if (res.x.max == res.x.min) {
                    res.x.max += FIX32(8);
                }

                start = FALSE;
            }

        }
    }
    return res;
}

bool engine_isOverlappingAxisLines(AxisLineF32 line1, AxisLineF32 line2) {
    return line1.max >= line2.min && line2.max >= line1.min;
}

bool engine_isOverlappingAABBs(AABB aabb1, AABB aabb2) {
    return (aabb1.x.max >= aabb2.x.min && aabb2.x.max >= aabb1.x.min) && (aabb1.y.max >= aabb2.y.min && aabb2.y.max >= aabb1.y.min);
}

AABB engine_getNewAABB(AABB aabb, Vect2D_s8 direction) {
    AABB newPlayerAABB;
    newPlayerAABB.x.min = aabb.x.min + intToFix32(direction.x);
    newPlayerAABB.x.max = aabb.x.max + intToFix32(direction.x);
    newPlayerAABB.y.min = aabb.y.min + intToFix32(direction.y);
    newPlayerAABB.y.max = aabb.y.max + intToFix32(direction.y);
    engine_initAABBTileIndexes(&newPlayerAABB);
    return newPlayerAABB;
}

AABB engine_getTopAABB(AABB aabb) {
    AABB aabbTop;
    aabbTop.x.min = aabb.x.min;
    aabbTop.x.max = engine_roundUpByEight(aabb.x.max);
    aabbTop.y.min = aabb.y.min - FIX32(8);
    aabbTop.y.max = engine_roundUpByEight(aabb.y.min);
    engine_initAABBTileIndexes(&aabbTop);
    return aabbTop;
}

AABB engine_getBottomAABB(AABB aabb) {
    AABB aabbBottom;
    aabbBottom.x.min = aabb.x.min;
    aabbBottom.x.max = engine_roundUpByEight(aabb.x.max);
    aabbBottom.y.min = aabb.y.max;
    aabbBottom.y.max = engine_roundUpByEight(aabb.y.max + FIX32(8));
    engine_initAABBTileIndexes(&aabbBottom);
    return aabbBottom;
}

AABB engine_getLeftAABB(AABB aabb) {
    AABB aabbLeft;
    aabbLeft.x.min = aabb.x.min - FIX32(8);
    aabbLeft.x.max = engine_roundUpByEight(aabb.x.min);
    aabbLeft.y.min = aabb.y.min;
    aabbLeft.y.max = engine_roundUpByEight(aabb.y.max);
    engine_initAABBTileIndexes(&aabbLeft);
    return aabbLeft;
}

AABB engine_getRightAABB(AABB aabb) {
    AABB aabbRight;
    aabbRight.x.min = aabb.x.max;
    aabbRight.x.max = engine_roundUpByEight(aabb.x.max + FIX32(8));
    aabbRight.y.min = aabb.y.min;
    aabbRight.y.max = engine_roundUpByEight(aabb.y.max);
    engine_initAABBTileIndexes(&aabbRight);
    return aabbRight;
}

void engine_initAABBTileIndexes(AABB* aabb) {
    (*aabb).xTiles.min = (*aabb).x.min / FIX32(8);
    (*aabb).xTiles.max = (*aabb).x.max / FIX32(8);
    (*aabb).yTiles.min = (*aabb).y.min / FIX32(8);
    (*aabb).yTiles.max = (*aabb).y.max / FIX32(8);

    //(*aabb).xTiles.min = fix32ToInt(fix32Div((*aabb).x.min, FIX32(8)));
    //(*aabb).xTiles.max = fix32ToInt(fix32Div((*aabb).x.max, FIX32(8)));
    //(*aabb).yTiles.min = fix32ToInt(fix32Div((*aabb).y.min, FIX32(8)));
    //(*aabb).yTiles.max = fix32ToInt(fix32Div((*aabb).y.max, FIX32(8)));
}

f32 engine_roundUpByEight(f32 x) {
    return (x + FIX32(7)) & FIX32(-8);
}