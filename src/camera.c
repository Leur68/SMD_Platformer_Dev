#include "../inc/global.h"

void camera_init() {
    cameraPosition.x = 0;
    cameraPosition.y = MAP_MAX_CAMERA_POS_Y;

    backPosition.x = 0;
    backPosition.y = BACK_MAX_CAMERA_POS_Y;
}

void camera_update() {
    //  If the player is currently within a NOT_SCROLLABLE zone of the camera:
    //      If the next camera position goes beyond the NOT_SCROLLABLE zone:
    //          Move the camera by the requested number of pixels
    //      If the next camera position does NOT go beyond the NOT_SCROLLABLE zone:
    //          Do nothing
    //  If the player is NOT within a NOT_SCROLLABLE zone:
    //      Do nothing

    // Initialize the initial screenPos so that, combined with movedPixels,
    // this allows camera_update() to make the correct scrolling decision.
    player->screenPos.x = SPR_getPositionX(player->sprite);
    player->screenPos.y = SPR_getPositionY(player->sprite);

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

    SPR_setPosition(player->sprite, player->globalAABB.x.min - cameraPosition.x, player->globalAABB.y.min - cameraPosition.y);
}

void camera_mustScrollByX(s16 v) {
    s16 posTemp = cameraPosition.x + v;
    if (posTemp < 0) {
        posTemp = 0;
    } else if (posTemp > mapMaxCameraPosX) {
        posTemp = mapMaxCameraPosX;
    }
    cameraPosition.x = posTemp;
    backPosition.x = cameraPosition.x * PARALLAX_RATIO_X;
    scrolled = true; // for condition in environment_update()
    MAP_scrollTo(map, cameraPosition.x, cameraPosition.y);
    MAP_scrollTo(back, backPosition.x, backPosition.y);
}

void camera_mustScrollByY(s16 v) {
    s16 posTemp = cameraPosition.y + v;
    if (posTemp < 0) {
        posTemp = 0;
    } else if (posTemp > mapMaxCameraPosY) {
        posTemp = mapMaxCameraPosY;
    }
    cameraPosition.y = posTemp;
    backPosition.y = cameraPosition.y * PARALLAX_RATIO_Y;
    scrolled = true; // for condition in environment_update()
    MAP_scrollTo(map, cameraPosition.x, cameraPosition.y);
    MAP_scrollTo(back, backPosition.x, backPosition.y);
}