#include "../inc/global.h"

void global_init() {
    VDP_setScreenWidth320();
	SPR_init();
    if (!SYS_isChecksumOk() && RELEASE) {
        while (1)
            VDP_init(); // haha
    }
}

// resetType это параметр главной функции игры
void disableSoftReset(bool resetType) {
    if (!resetType)
        SYS_hardReset();
}

void fadeInImage(Image img, u16 numFrame) {
    PAL_setColors(0, palette_black, 64, DMA);               // Устанавливаем чёрный экран
    PAL_fadeIn(0, 63, img.palette->data, numFrame, true);   // Производим эффект FadeIn для нужной палитры
}

void fadeInPalette(const u16* pal, u16 numFrame) {
    PAL_setColors(0, palette_black, 64, DMA);               // Устанавливаем чёрный экран
    PAL_fadeIn(0, 63, pal, numFrame, true);                 // Производим эффект FadeIn для нужной палитры
}

void fadeInScreen(u16 numFrame) {
    u16 palette_full[64];                                   // Буфер для хранения содержимого CRAM
    PAL_getColors(0, palette_full, 64);                     // Копируем в буфер содержимое CRAM (перед следующим вызовом, т.к. иначе скопировать не получится)
    PAL_setColors(0, palette_black, 64, DMA);               // Устанавливаем чёрный экран
    PAL_fadeIn(0, 63, palette_full, numFrame, true);        // Производим эффект FadeIn для нужной палитры
}

void fadeOutScreen(u16 numFrame) {
    PAL_fadeOut(0, 63, numFrame, false);                    // Производим эффект FadeOut для всего экрана. async = false, чтобы не происходил преждевременный переход к следующей сцене
}

void drawInt(s32 num, u16 x, u16 y, u16 len) {
    char result[len + 1];
    intToStr(num, result, 1);
    VDP_drawTextFill(result, x, y, len);
}

void drawFix(f32 num, u16 x, u16 y, u16 len) {
    char result[len + 1];
    fix32ToStr(num, result, 1);
    VDP_drawTextFill(result, x, y, len);
}