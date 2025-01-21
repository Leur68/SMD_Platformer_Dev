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

// Palettes
#define TEXT_PALETTE           PAL0
#define BACKGROUND_PALETTE     PAL1
#define GROUND_PALETTE         PAL2
#define PLAYER_PALETTE         PAL3
#define DEBUG_PALETTE          PAL0

// Planes
#define GROUND_PLANE           BG_A
#define BACKGROUND_PLANE       BG_B
#define TEXT_PLANE             WINDOW

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

typedef enum
{
    STATE_INTRO,   // A series of intros shown when the game starts
    STATE_START,   // Game start and settings screen
    STATE_GAME,    // Gameplay state
    STATE_OPTIONS, // Settings menu
    STATE_CREDITS, // Credits screen
} GameState;

extern GameState currentState;                                         // The current state of the game
extern bool paused;                                                    // Indicates whether the game is paused
extern Player *player;                                                 // Pointer to the player object
extern Map *map;                                                       // Pointer to the map (foreground layer)
extern Map *back;                                                      // Pointer to the background map (background layer)
extern u16 mapW;                                                       // Width of the map in pixels
extern u16 mapH;                                                       // Height of the map in pixels
extern u16 mapWTiles;                                                  // Width of the map in tiles
extern u16 mapHTiles;                                                  // Height of the map in tiles
extern u16 mapMaxCameraPosX;                                           // Maximum camera position on the X-axis for the map
extern u16 mapMaxCameraPosY;                                           // Maximum camera position on the Y-axis for the map
extern u16 backMaxCameraPosX;                                          // Maximum camera position on the X-axis for the background
extern u16 backMaxCameraPosY;                                          // Maximum camera position on the Y-axis for the background
extern Vect2D_u16 cameraPosition;                                      // Current position of the camera on the map
extern Vect2D_u16 backPosition;                                        // Current position of the camera on the background
extern bool scrolled;                                                  // Indicates if the map was scrolled during the current frame
extern u8 *collisionsMap;                                              // Pointer to the collision map data
extern u16 score;                                                      // Current player score
extern u8 lastLoadedObject;                                            // Index of the last object that was loaded
extern GameObject *objects[];                                          // Array of all game objects
extern GameObject *collidedObject;                                     // Object that the player collided with in the current frame
extern u8 currObjectIndex;                                             // Index of the object currently being processed in `environment_update()`
extern GameObject *currObject;                                         // Object currently being processed in `environment_update()`
extern void (*environment_initObject)(void);                           // Pointer to the function that initializes an object
extern void (*environment_initObjectSprite)(void);                     // Pointer to the function that initializes an object's sprite
extern void (*environment_onUpdateObject)(void);                       // Pointer to the function that updates an object (currObject) in the current frame
extern void (*environment_onUpdateObjectInViewport)(void);             // Pointer to the function that updates an object (currObject) in the current frame within the viewport
extern void (*environment_onObjectCollidesWithPlayerInViewport)(void); // Pointer to the function that handles collisions between the player and an object (currObject) in the current frame within the viewport