#include "../inc/game.h"

Player* player;
Map* map;
Map* back;
u16 mapW;
u16 mapH;
u16 mapWTiles;
u16 mapHTiles;
Vect2D_u16 cameraPosition;
Vect2D_u16 backPosition;
s16 mapOverheight;
s16 backOverheight;
u8* collisionsMap;

u16 score;