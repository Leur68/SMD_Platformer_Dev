#include "../inc/global.h"

GameObject* allocGameObject() {
    return (GameObject*) MEM_alloc(sizeof(GameObject));
}

void environment_init(u8* collisions) {
    collisionsMap = collisions;

    for (u16 x = 0; x < mapWTiles; x++) {
        for (u16 y = 0; y < mapHTiles; y++) {
            u8 objType = mapPointerGet(collisionsMap, x, y);
            if (objType > 1) {
                currObject = allocGameObject();

                u16 globalPosX = (x << 3);
                u16 globalPosY = (y << 3);

                currObject->objType = objType;
                currObject->visible = true;
                currObject->globalAABB.x.min = globalPosX;
                currObject->globalAABB.y.min = globalPosY;

                environment_initObject();
                environment_initObjectSprite();

                currObject->globalAABB.x.max = globalPosX + currObject->sprite->definition->w;
                currObject->globalAABB.y.max = globalPosY + currObject->sprite->definition->h;

                currObject->screenPos.x = currObject->globalAABB.x.min - cameraPosition.x;
                currObject->screenPos.y = currObject->globalAABB.y.min - cameraPosition.y;

                objects[lastLoadedObject] = currObject;

                lastLoadedObject++;
            }
        }
    }
}

void environment_update() {
    for (currObjectIndex = 0; currObjectIndex < lastLoadedObject; currObjectIndex++) {
        currObject = objects[currObjectIndex];

        if (currObject == NULL) {
            continue;
        }
                
        environment_onUpdateObject();

        bool changedPos = currObject->objType == M_PLATFORM_TILE_INDEX || scrolled;

        if (changedPos) {
            currObject->screenPos.x = currObject->globalAABB.x.min - cameraPosition.x;
            currObject->screenPos.y = currObject->globalAABB.y.min - cameraPosition.y;
        }
        
        if (isCurrObjectVisible) {
            // Если спрайт в зоне видимости, но ранее был скрыт, восстанавливаем
            if (currObject->visible == false) {
                environment_initObjectSprite();
                currObject->visible = true;
            }

            environment_onUpdateObjectInViewport();
            
            // Если произошло столкновение со спрайтом
            if (hasCurrObjectCollidesWithPlayer) {

                environment_onObjectCollidesWithPlayerInViewport();

            }

            if (changedPos) {
                // Если объект не был удален, обновляем позицию спрайта на экране
                if (objects[currObjectIndex] != NULL) {
                    SPR_setPosition(currObject->sprite, currObject->screenPos.x, currObject->screenPos.y); 
                }
            }
        } else {
            // Если спрайт вышел за пределы экрана, временно удаляем его из памяти
            if (currObject->visible == true) {
                SPR_releaseSprite(currObject->sprite);
                currObject->visible = false;
            }
        }
    }
}

void environment_objectDelete() {
    SPR_releaseSprite(currObject->sprite);
    MEM_free(currObject);
    objects[currObjectIndex] = NULL;
}