#include "../inc/global.h"

u8 hearts[] = {0, 0, 0, 0, 0};
Sprite* sprites[5];

void hud_init() {
    // window plane
    VDP_setWindowOnTop(1); // Top panel with information

    // text plane and palette
    VDP_setTextPlane(TEXT_PLANE);
    VDP_setTextPalette(TEXT_PALETTE);

    // hp
    hud_updateHP(20);

    // score
    VDP_drawText("Score: ", 10, 0);
    hud_updateScore(0);
}

void hud_updateHP(s16 val) {
    if (val == hp || val < 0) {
        return;
    }
    hp = val;

    hearts[0] = max(0, min(hp, 4) - 1 + 1);   // [1,4]
    hearts[1] = max(0, min(hp, 8) - 5 + 1);   // [5,8]
    hearts[2] = max(0, min(hp, 12) - 9 + 1);  // [9,12]
    hearts[3] = max(0, min(hp, 16) - 13 + 1); // [13,16]
    hearts[4] = max(0, min(hp, 20) - 17 + 1); // [17,20]

    hud_updateHpSprites();
}

void hud_updateHpSprites() {
    for (size_t i = 0; i < 5; i++) {
        u8 currHp = hearts[i];
        Sprite *currSprite = sprites[i];

        if (currHp == 0) {
            if (currSprite != NULL) {
                SPR_releaseSprite(currSprite); // We assume that if it's 0, then the sprite has already been initialized previously
                sprites[i] = NULL;
            }
        } else {
            if (currSprite == NULL) {
                currSprite = SPR_addSpriteSafe(&heart_sprite, ((29 * 8) + (i * 16)), 0, TILE_ATTR(PLAYER_PALETTE, 0, false, false));
                sprites[i] = currSprite;
            }
            SPR_setAnim(currSprite, currHp - 1);
        }
        
    }
}

void hud_updateScore(s16 val) {
    if (val == score || val < 0) {
        return;
    }
    score = val;

    char str[2];
    intToStr(score, &str, 1);
    VDP_drawText(str, 17, 0);
}