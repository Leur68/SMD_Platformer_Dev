#pragma once
#include <genesis.h>


#define SCREEN_WIDTH           320
#define SCREEN_HEIGHT          224
#define SCREEN_WIDTH_TILES     (SCREEN_WIDTH  / 8)  // 40
#define SCREEN_HEIGHT_TILES    (SCREEN_HEIGHT / 8)  // 28

#define DIRECTION_NONE         0b00000000  // 0
#define DIRECTION_LEFT         0b00000001  // 1
#define DIRECTION_RIGHT        0b00000010  // 2
#define DIRECTION_UP           0b00000100  // 4
#define DIRECTION_DOWN         0b00001000  // 8
#define DIRECTION_LEFT_UP      (DIRECTION_LEFT | DIRECTION_UP)    // 0b00000101 = 5
#define DIRECTION_LEFT_DOWN    (DIRECTION_LEFT | DIRECTION_DOWN)  // 0b00001001 = 9
#define DIRECTION_RIGHT_UP     (DIRECTION_RIGHT | DIRECTION_UP)   // 0b00000110 = 6
#define DIRECTION_RIGHT_DOWN   (DIRECTION_RIGHT | DIRECTION_DOWN) // 0b00001010 = 10

// Палитры
#define TEXT_PALETTE              PAL0
#define BACKGROUND_PALETTE        PAL1
#define GROUND_PALETTE            PAL2
#define PLAYER_PALETTE            PAL3
#define DEBUG_PALETTE             PAL0

// Плейны
#define GROUND_PLANE              BG_A
#define BACKGROUND_PLANE          BG_B
#define TEXT_PLANE                WINDOW

#include "math.h"
#include "aabb.h"
#include "collision.h"
#include "debug.h"
#include "environment.h"
#include "util.h"

#include "intro.h"
#include "start.h"
#include "options.h"
#include "game.h"

#include "player.h"
#include "camera.h"

#include "../res/resources.h"

typedef enum {
    STATE_INTRO,   // Ряд заставок, отображаемых при запуске игры
    STATE_START,   // Экран запуска игры и настроек
    STATE_GAME,  // Игра
    STATE_OPTIONS, // Настройки
    STATE_CREDITS, // Титры
} GameState;

extern GameState currentState;

extern bool paused;
extern bool xyzButtons;

extern Player* player;
extern Map* map;
extern Map* back;
extern u16 mapW;
extern u16 mapH;
extern u16 mapWTiles;
extern u16 mapHTiles;
extern u16 mapMaxCameraPosX;
extern u16 mapMaxCameraPosY;
extern u16 backMaxCameraPosX;
extern u16 backMaxCameraPosY;
extern Vect2D_u16 cameraPosition;
extern Vect2D_u16 backPosition;
extern bool scrolled;
extern u8* collisionsMap;
extern u16 score;
extern u8 lastLoadedObject;
extern u8 currObjectIndex;
extern GameObject* objects[];
extern GameObject* currObject; // объект который на текущем фрейме в данный момент обрабатывается (environment_update())
extern void (*environment_initObject) (void);
extern void (*environment_initObjectSprite) (void);
extern void (*environment_onUpdateObject) (void);
extern void (*environment_onUpdateObjectInViewport) (void);
extern void (*environment_onObjectCollidesWithPlayerInViewport) (void);