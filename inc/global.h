#include <genesis.h>

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
extern Vect2D_s16 cameraPosition;
extern Vect2D_s16 backPosition;
extern bool scrolled;
extern u8* collisionsMap;
extern u16 score;
extern u8 lastLoadedObject;
extern u8 currObjectIndex;
extern GameObject* objects[];
extern GameObject* currObject;
extern void (*environment_initObject) (void);
extern void (*environment_initObjectSprite) (void);
extern void (*environment_onUpdateObject) (void);
extern void (*environment_onUpdateObjectInViewport) (void);
extern void (*environment_onObjectCollidesWithCharacterInViewport) (void);
