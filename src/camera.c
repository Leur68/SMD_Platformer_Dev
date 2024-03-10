#include "../inc/game.h"

void camera_init() {
    cameraPosition.x = 0;
    cameraPosition.y = MAP_OVERHEIGHT;

    backPosition.x = FASTFIX32(0);
    backPosition.y = FASTFIX32(BACK_OVERHEIGHT);
}

void camera_update() {
    bool isScrollX = (
        player->movedPixels.x != 0 &&                                                                                                    // Скроллим карту только при движении персонажа

        (player->facingDirection.x == DIRECTION_RIGHT || (cameraPosition.x + player->movedPixels.x) >= 0) &&                             // Чтобы не смещать за левую границу
        (player->facingDirection.x == DIRECTION_LEFT  || (cameraPosition.x + player->movedPixels.x) <= (mapW - SCREEN_WIDTH)) &&         // Чтобы не смещать за правую границу
        
        ((player->facingDirection.x == DIRECTION_LEFT && player->screenPos.x <= SCREEN_MIN_X) ||                                         // Чтобы смещать влево
        (player->facingDirection.x == DIRECTION_RIGHT && (player->screenPos.x + PLAYER_WIDTH) >= SCREEN_MAX_X))                          // Чтобы смещать вправо
    );
    bool isScrollY = (
        player->movedPixels.y != 0 &&                                                                                                    // Скроллим карту только при движении персонажа

        (player->facingDirection.y == DIRECTION_DOWN || (cameraPosition.y + player->movedPixels.y) >= 0) &&                              // Чтобы не смещать за верхнюю границу
        (player->facingDirection.y == DIRECTION_UP   || (cameraPosition.y + player->movedPixels.y) <= (mapH - SCREEN_HEIGHT)) &&         // Чтобы не смещать за нижнюю границу
        
        ((player->facingDirection.y == DIRECTION_UP && player->screenPos.y <= SCREEN_MIN_Y) ||                                           // Чтобы смещать вверх
        (player->facingDirection.y == DIRECTION_DOWN && player->screenPos.y + PLAYER_HEIGHT >= SCREEN_MAX_Y))                            // Чтобы смещать вниз
    );

    // TODO Нужно что-то сделать с отрицательными значениями cameraPosition
    u16 scrollX = isScrollX ? player->movedPixels.x : 0;
    u16 scrollY = isScrollY ? player->movedPixels.y : 0;

    if (scrollX != 0 || scrollY != 0) {
        cameraPosition.x += scrollX;
        cameraPosition.y += scrollY;
        
        backPosition.x += fastFix32Div(FASTFIX32(scrollX), FASTFIX32(6.0)); 
        backPosition.y += fastFix32Div(FASTFIX32(scrollY), FASTFIX32(3.3));

        MAP_scrollTo(map, cameraPosition.x, cameraPosition.y);
        MAP_scrollTo(back, fastFix32ToInt(backPosition.x), fastFix32ToInt(backPosition.y));
    }
}