#include "global.h"

GameState currentState;
bool paused = false;
Player *player;
Map *map;
Map *back;
u16 mapW;
u16 mapH;
u16 mapWTiles;
u16 mapHTiles;
u16 mapMaxCameraPosX;
u16 mapMaxCameraPosY;
u16 backMaxCameraPosX;
u16 backMaxCameraPosY;
Vect2D_u16 cameraPosition;
Vect2D_u16 backPosition;
bool scrolled;
u8 *collisionsMap;
Pool *objectsPool = NULL;
GameObject *collidedObject;
void (*environment_initObject)(GameObject *object);
void (*environment_initObjectSprite)(GameObject *object);
void (*environment_onUpdateObject)(GameObject *object);
void (*environment_onUpdateObjectInViewport)(GameObject *object);
void (*environment_onObjectCollidesWithPlayerInViewport)(GameObject *object);
void (*environment_onTileCollidesWithPlayerInViewport)(void);
u16 score = -1;
u16 hp = -1;