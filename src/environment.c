#include "global.h"

GameObject *allocGameObject() {
    return POOL_allocate(objectsPool);
}

void environment_init() {
    objectsPool = POOL_create(GAME_MAX_OBJECTS, sizeof(GameObject));

    for (u16 x = 0; x < mapWTiles; x++) {
        for (u16 y = 0; y < mapHTiles; y++) {
            u8 tileIndex = mapPointerGet(collisionsMap, x, y);
            if (IS_OBJECT(tileIndex)) {
                GameObject *currObject = allocGameObject();

                u16 globalPosX = (x << 3);
                u16 globalPosY = (y << 3);

                currObject->tileIndex = tileIndex;
                currObject->visible = true;

                environment_initObject(currObject);

                currObject->globalAABB.x.min = globalPosX;
                currObject->globalAABB.y.min = globalPosY;

                environment_initObjectSprite(currObject);
                
                currObject->globalAABB.x.max = globalPosX + currObject->sprite->definition->w;

                if (IS_TOP_SOLID_OBJECT(tileIndex)) {
                    currObject->globalAABB.y.max = globalPosY + 7; // если поставить 8, персонаж сможет стоять ниже поверхности платформы
                } else {
                    currObject->globalAABB.y.max = globalPosY + currObject->sprite->definition->h;
                }

            }
        }
    }
}

void environment_updateObjects() {

    GameObject** objects = (GameObject**) POOL_getFirst(objectsPool);
    u16 num = POOL_getNumAllocated(objectsPool);
    while (num--) {
        GameObject *object = *objects++;

        environment_onUpdateObject(object);
    }
}

void environment_freeObject(GameObject *object) {
    SPR_releaseSprite(object->sprite);
    POOL_release(objectsPool, object, true);
}

void environment_cleanup() {
    GameObject** objects = (GameObject**) POOL_getFirst(objectsPool);
    u16 num = POOL_getNumAllocated(objectsPool);
    while (num--) {
        GameObject *currObject = *objects++;
        environment_freeObject(currObject);
    }
    POOL_destroy(objectsPool);
    MEM_free(collisionsMap);
}

void environment_updateSprites() {
    GameObject** objects = (GameObject**) POOL_getFirst(objectsPool);
    u16 num = POOL_getNumAllocated(objectsPool);
    while (num--) {
        GameObject *currObject = *objects++;

        s16 currObjectScreenX = currObject->globalAABB.x.min - cameraPosition.x;
        s16 currObjectScreenY = currObject->globalAABB.y.min - cameraPosition.y;

        if (isObjectVisible(currObjectScreenX, currObjectScreenY, currObject->sprite->definition->w, currObject->sprite->definition->h)) {
            // If the sprite is within the visible area but was previously hidden, restore it
            if (currObject->visible == false) {
                environment_initObjectSprite(currObject);
                currObject->visible = true;
            }

            environment_onUpdateObjectInViewport(currObject);

            bool changedPos = IS_MOVING_PLATFORM(currObject->tileIndex) || scrolled;
            if (changedPos) {
                // If the object was not deleted, update the sprite's position on the screen
                if (currObject != NULL) { // The object might be deleted during callbacks
                    SPR_setPosition(currObject->sprite, currObjectScreenX, currObjectScreenY);
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