#include "../inc/global.h"

void camera_init() {
    cameraPosition.x = 0;
    cameraPosition.y = MAP_MAX_CAMERA_POS_Y;

    backPosition.x = 0;
    backPosition.y = BACK_MAX_CAMERA_POS_Y;
}

void camera_update() {
    //  Если персонаж на текущем положении камеры находится в NOT_SCROLLABLE зоне
    //      Если следующее положение камеры выходит за NOT_SCROLLABLE зону
    //          Перемещаем камеру на запрошенное кол-во пикселей
    //      Если следующее положение камеры НЕ выходит за NOT_SCROLLABLE зону
    //          Не делаем ничего
    //  Если персонаж НЕ находится в NOT_SCROLLABLE зоне
    //      Не делаем ничего

    scrolled = false;

    if (hasPlayerMovedByX) {
        if (currScreenPosInScrollableX) {
            if (nextScreenPosInNotScrollableX) {
                scrolled = true;

                s16 posTemp = cameraPosition.x + player->movedPixels.x;
                if (posTemp < 0) {
                    posTemp = 0;
                } else if (posTemp > mapMaxCameraPosX) {
                    posTemp = mapMaxCameraPosX;
                }
                cameraPosition.x = posTemp;

                backPosition.x = cameraPosition.x * PARALLAX_RATIO_X;
            }
        }
    }
    
    if (hasPlayerMovedByY) {
        if (currScreenPosInScrollableY) {
            if (nextScreenPosInNotScrollableY) {
                scrolled = true;

                s16 posTemp = cameraPosition.y + player->movedPixels.y;
                if (posTemp < 0) {
                    posTemp = 0;
                } else if (posTemp > mapMaxCameraPosY) {
                    posTemp = mapMaxCameraPosY;
                }
                cameraPosition.y = posTemp;

                backPosition.y = cameraPosition.y * PARALLAX_RATIO_Y;
            }
        }
    }

    if (scrolled) {
        MAP_scrollTo(map, cameraPosition.x, cameraPosition.y);
        MAP_scrollTo(back, backPosition.x, backPosition.y);
    }

}