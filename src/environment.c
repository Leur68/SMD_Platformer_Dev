#include "../inc/global.h"

GameObject *allocGameObject() {
    return (GameObject *)MEM_alloc(sizeof(GameObject));
}

void environment_init(u8 *collisions) {
    collisionsMap = collisions;

    for (u16 x = 0; x < mapWTiles; x++) {
        for (u16 y = 0; y < mapHTiles; y++) {
            u8 tileIndex = mapPointerGet(collisionsMap, x, y);
            if (tileIndex == BONUS_TILE_INDEX || tileIndex == M_PLATFORM_TILE_INDEX) {
                currObject = allocGameObject();

                u16 globalPosX = (x << 3);
                u16 globalPosY = (y << 3);

                currObject->objType = tileIndex;
                currObject->visible = true;
                currObject->globalAABB.x.min = globalPosX;
                currObject->globalAABB.y.min = globalPosY;

                environment_initObject();
                environment_initObjectSprite();

                currObject->globalAABB.x.max = globalPosX + currObject->sprite->definition->w;
                currObject->globalAABB.y.max = globalPosY + currObject->sprite->definition->h;

                objects[lastLoadedObject] = currObject;

                lastLoadedObject++;
            }
        }
    }
}

void environment_updateObjects() {
    collidedObject = NULL;

    for (currObjectIndex = 0; currObjectIndex < lastLoadedObject; currObjectIndex++) {
        currObject = objects[currObjectIndex];

        if (currObject == NULL) {
            continue;
        }

        environment_onUpdateObject();

        if (hasCurrObjectCollidesWithPlayer) {
            
            if (currObject->objType == M_PLATFORM_TILE_INDEX) {
                collidedObject = currObject;
            }
            environment_onObjectCollidesWithPlayerInViewport();
        }
    }
}

void environment_freeObject() {
    SPR_releaseSprite(currObject->sprite);
    MEM_free(currObject);
    objects[currObjectIndex] = NULL;
}

void environment_cleanup() {
    for (u8 i = 0; i < lastLoadedObject; i++) {
        currObject = objects[i];

        if (currObject == NULL) {
            continue;
        }

        environment_freeObject();
    }

    MEM_free(objects);
    MEM_free(collisionsMap);
}

void environment_updateSprites() {
    for (currObjectIndex = 0; currObjectIndex < lastLoadedObject; currObjectIndex++) {
        currObject = objects[currObjectIndex];

        if (currObject == NULL) {
            continue;
        }

        s16 playerScreenX = currObject->globalAABB.x.min - cameraPosition.x;
        s16 playerScreenY = currObject->globalAABB.y.min - cameraPosition.y;

        if (isCurrObjectVisible(playerScreenX, playerScreenY)) {
            // If the sprite is within the visible area but was previously hidden, restore it
            if (currObject->visible == false) {
                environment_initObjectSprite();
                currObject->visible = true;
            }

            environment_onUpdateObjectInViewport();

            bool changedPos = currObject->objType == M_PLATFORM_TILE_INDEX || scrolled;
            if (changedPos) {
                // If the object was not deleted, update the sprite's position on the screen
                if (objects[currObjectIndex] != NULL) { // The object might be deleted during callbacks
                    SPR_setPosition(currObject->sprite, playerScreenX, playerScreenY);
                }
            }
        } else {
            // If the sprite goes out of the screen boundaries, temporarily remove it from memory
            if (currObject->visible == true) {
                SPR_releaseSprite(currObject->sprite);
                currObject->visible = false;
#if DEBUG_KDEBUG_LOG
                KDebug_Alert("Object sprite released");
#endif
            }
        }
        
    }
}