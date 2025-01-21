#include "../inc/global.h"

void global_init()
{
    VDP_setScreenWidth320();
    SPR_init();
    if (!SYS_isChecksumOk() && RELEASE)
    {
        while (1)
            VDP_init(); // haha
    }
}

void disableSoftReset(bool resetType)
{
    if (!resetType)
        SYS_hardReset();
}

void fadeInImage(Image img, u16 numFrame)
{
    PAL_setColors(0, palette_black, 64, DMA);             // Set the screen to black
    PAL_fadeIn(0, 63, img.palette->data, numFrame, true); // Perform a fade-in effect for the desired palette
}

void fadeInPalette(const u16 *pal, u16 numFrame)
{
    PAL_setColors(0, palette_black, 64, DMA); // Set the screen to black
    PAL_fadeIn(0, 63, pal, numFrame, true);   // Perform a fade-in effect for the desired palette
}

void fadeInScreen(u16 numFrame)
{
    u16 palette_full[64];                            // Buffer to store the contents of CRAM
    PAL_getColors(0, palette_full, 64);              // Copy the contents of CRAM into the buffer (this must be done before the next call, otherwise it won't be possible to copy)
    PAL_setColors(0, palette_black, 64, DMA);        // Set the screen to black
    PAL_fadeIn(0, 63, palette_full, numFrame, true); // Perform a fade-in effect for the current palette
}

void fadeOutScreen(u16 numFrame)
{
    PAL_fadeOut(0, 63, numFrame, false); // Perform a fade-out effect for the entire screen. async = false to prevent an early transition to the next scene
}

void drawInt(s32 num, u16 x, u16 y, u16 len)
{
    char result[len + 1];
    intToStr(num, result, 1);
    VDP_drawTextFill(result, x, y, len);
}

void drawFix(f32 num, u16 x, u16 y, u16 len)
{
    char result[len + 1];
    fix32ToStr(num, result, 1);
    VDP_drawTextFill(result, x, y, len);
}