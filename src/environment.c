#include "global.h"

GameObject *allocGameObject() {
    return POOL_allocate(objectsPool);
}

void environment_init() {
    objectsPool = POOL_create(GAME_MAX_OBJECTS, sizeof(GameObject));

    for (u16 x = 0; x < mapWTiles; x++) {
        for (u16 y = 0; y < mapHTiles; y++) {
            u8 tileIndex = mapPointerGet(collisionsMap, x, y);
            if (tileIndex == BONUS_TILE_INDEX || tileIndex == M_X_PLATFORM_TILE_INDEX || tileIndex == M_Y_PLATFORM_TILE_INDEX || tileIndex == M_XY_PLATFORM_TILE_INDEX) {
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
            }
        }
    }
}

void environment_updateObjects() {

    GameObject** objects = (GameObject**) POOL_getFirst(objectsPool);
    u16 num = POOL_getNumAllocated(objectsPool);
    while (num--) {
        currObject = *objects++;

        bool intersects = aabb_intersects(player->collider->globalAABB, currObject->globalAABB);

        environment_onUpdateObject();

        // hasCurrObjectCollidesWithPlayer
        if (intersects) {
            
            if (currObject->objType == M_X_PLATFORM_TILE_INDEX || currObject->objType == M_Y_PLATFORM_TILE_INDEX || currObject->objType == M_XY_PLATFORM_TILE_INDEX) {
                collidedObject = currObject;
            }
            environment_onObjectCollidesWithPlayerInViewport();
        }
    }
    
    if (collidedObject == NULL) {
        return;
    }

    u8 bottom   = GET_BOTTOM_COLLISION(player->collider); // чтобы платформа не двигала песонажа если он ударился головой об нее
    bool ground = HAS_GROUND_COLLISION(player->collider); // чтобы персонаж не "прилипал" к платформе если он стоит на земле, а она столкнулась с ним
        
    if (bottom && !ground) {
        if (collidedObject->moving.x != 0) {
            player->autoVelocity.x = FASTFIX32(collidedObject->moving.x);
        }
        if (collidedObject->moving.y != 0) {
            player->autoVelocity.y = FASTFIX32(collidedObject->moving.y);
        }
    }
}

void environment_freeObject() {
    SPR_releaseSprite(currObject->sprite);
    POOL_release(objectsPool, currObject, true);
    currObject = NULL;
}

void environment_cleanup() {
    GameObject** objects = (GameObject**) POOL_getFirst(objectsPool);
    u16 num = POOL_getNumAllocated(objectsPool);
    while (num--) {
        currObject = *objects++;
        environment_freeObject();
    }
    POOL_destroy(objectsPool);
    MEM_free(collisionsMap);
}

void environment_updateSprites() {
    GameObject** objects = (GameObject**) POOL_getFirst(objectsPool);
    u16 num = POOL_getNumAllocated(objectsPool);
    while (num--) {
        currObject = *objects++;

        s16 currObjectScreenX = currObject->globalAABB.x.min - cameraPosition.x;
        s16 currObjectScreenY = currObject->globalAABB.y.min - cameraPosition.y;

        if (isObjectVisible(currObjectScreenX, currObjectScreenY, currObject->sprite->definition->w, currObject->sprite->definition->h)) {
            // If the sprite is within the visible area but was previously hidden, restore it
            if (currObject->visible == false) {
                environment_initObjectSprite();
                currObject->visible = true;
            }

            environment_onUpdateObjectInViewport();

            bool changedPos = currObject->objType == M_X_PLATFORM_TILE_INDEX || currObject->objType == M_Y_PLATFORM_TILE_INDEX || currObject->objType == M_XY_PLATFORM_TILE_INDEX || scrolled;
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