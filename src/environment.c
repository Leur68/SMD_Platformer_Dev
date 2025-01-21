#include "../inc/global.h"

GameObject *allocGameObject()
{
    return (GameObject *)MEM_alloc(sizeof(GameObject));
}

void environment_init(u8 *collisions)
{
    collisionsMap = collisions;

    for (u16 x = 0; x < mapWTiles; x++)
    {
        for (u16 y = 0; y < mapHTiles; y++)
        {
            u8 objType = mapPointerGet(collisionsMap, x, y);
            if (objType > 1)
            {
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

void environment_update()
{
    collidedObject = NULL;

    for (currObjectIndex = 0; currObjectIndex < lastLoadedObject; currObjectIndex++)
    {
        currObject = objects[currObjectIndex];

        if (currObject == NULL)
        {
            continue;
        }

        environment_onUpdateObject();

        bool changedPos = currObject->objType == M_PLATFORM_TILE_INDEX || scrolled;

        if (changedPos)
        {
            currObject->screenPos.x = currObject->globalAABB.x.min - cameraPosition.x;
            currObject->screenPos.y = currObject->globalAABB.y.min - cameraPosition.y;
        }

        if (isCurrObjectVisible)
        {
            // If the sprite is within the visible area but was previously hidden, restore it
            if (currObject->visible == false)
            {
                environment_initObjectSprite();
                currObject->visible = true;
            }

            environment_onUpdateObjectInViewport();

            // If there was a collision with the sprite
            if (hasCurrObjectCollidesWithPlayer)
            {
                if (currObject->objType == M_PLATFORM_TILE_INDEX)
                {
                    collidedObject = currObject;
                }

                environment_onObjectCollidesWithPlayerInViewport();
            }

            if (changedPos)
            {
                // If the object was not deleted, update the sprite's position on the screen
                if (objects[currObjectIndex] != NULL)
                { // The object might be deleted during callbacks
                    SPR_setPosition(currObject->sprite, currObject->screenPos.x, currObject->screenPos.y);
                }
            }
        }
        else
        {
            // If the sprite goes out of the screen boundaries, temporarily remove it from memory
            if (currObject->visible == true)
            {
                SPR_releaseSprite(currObject->sprite);
                currObject->visible = false;
            }
        }
    }
}

void environment_objectDelete()
{
    SPR_releaseSprite(currObject->sprite);
    MEM_free(currObject);
    objects[currObjectIndex] = NULL;
}