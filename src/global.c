#include "../inc/game.h"

Player* player;
Map* map;
Map* back;
u16 mapW;
u16 mapH;
u16 mapWTiles;
u16 mapHTiles;
u16 mapMaxCameraPosX;
u16 mapMaxCameraPosY;
u16 backMaxCameraPosX;
u16 backMaxCameraPosY;
Vect2D_s16 cameraPosition;
Vect2D_s16 backPosition;
bool scrolled;
u8* collisionsMap;
u16 score;
u8 lastLoadedObject = 0;
u8 currObjectIndex = 0;
GameObject *currObject;
void (*environment_initObject) (void);
void (*environment_initObjectSprite) (void);
void (*environment_onUpdateObject) (void);
void (*environment_onUpdateObjectInViewport) (void);
void (*environment_onObjectCollidesWithPlayerInViewport) (void);