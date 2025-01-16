#include "../inc/global.h"

u8 collision_getTileIndex(u16 xTile, u16 yTile) {
    // Проверяем границы карты
    if (xTile < 0 || yTile < 0 || xTile >= mapWTiles || yTile >= mapHTiles) return true;
    // Возвращаем состояние тайла
    return mapPointerGet(collisionsMap, xTile, yTile);
}

bool collision_isTileSolid(u16 xTile, u16 yTile) {
    u8 index = collision_getTileIndex(xTile, yTile);
    if (index == M_PLATFORM_TILE_INDEX) {
        mapPointerSet(collisionsMap, xTile, yTile, 0);
        index = 0;
    }
    return index == SOLID_TILE_INDEX || index == M_PLATFORM_TILE_INDEX;
}

bool collision_checkMapArea(AABB aabb, AABB* result) {
    u16  xMin   = aabb.tileX.max;   // Минимальные значения x и y тайлов
    u16  yMin   = aabb.tileY.max;
    u16  xMax   = aabb.tileX.min;   // Максимальные значения x и y тайлов
    u16  yMax   = aabb.tileY.min;
    bool exists = false;

    // Проходим по каждому тайлу в заданной области
    for (u16 currYTile = aabb.tileY.min; currYTile < aabb.tileY.max; currYTile++) {
        for (u16 currXTile = aabb.tileX.min; currXTile < aabb.tileX.max; currXTile++) {
            // Проверяем, является ли тайл твердым (коллизия)
            if (collision_isTileSolid(currXTile, currYTile)) {
                // Определяем минимальные и максимальные координаты тайлов
                if (currXTile < xMin) xMin = currXTile;
                if (currYTile < yMin) yMin = currYTile;
                if (currXTile > xMax) xMax = currXTile;
                if (currYTile > yMax) yMax = currYTile;
                
                exists = true;
            }
        }
    }

    // Если найдены коллизии, вычисляем AABB
    if (exists) {
        result->tileX.min = xMin;
        result->tileY.min = yMin;
        result->tileX.max = xMax;
        result->tileY.max = yMax;

        // Переводим координаты из тайлов в пиксели
        result->x.min = xMin << 3;  // Умножение на 8 с помощью сдвига
        result->y.min = yMin << 3;
        result->x.max = (xMax << 3) + 8;
        result->y.max = (yMax << 3) + 8;
    }

    return exists;
}

void collision_check(AABB aabb, u8 direction, u8* left, u8* right, u8* top, u8* bottom) {

    *left = *right = *top = *bottom = 0;

    // Получаем AABB для проверки столкновений, после этого отсеиваем среди найденных тайлов только те, что являются твердыми на карте. Данными тайлами считаются те, вплотную к которым или внутри которых находятся соответствующие стороны AABB персонажа
    AABB aabbLeft;
    AABB aabbTop;
    AABB aabbRight;
    AABB aabbBottom;
    u8 h;
    u8 v;

    switch (direction) {
        // Двигаемся только влево
        case DIRECTION_LEFT: 
            // Проверяем столкновения в массиве коллизий
            aabbLeft = aabb_getLeftAABB(aabb);
        
            if (collision_checkMapArea(aabbLeft, &aabbLeft)) {
                *left = aabbLeft.x.max - aabb.x.min + 1;
            }
            // Проверяем столкновения с объектами
            if (сollidedObject != NULL && сollidedObject->globalAABB.x.max < aabb.x.max) {
                h = collision_getIntersectionLen(сollidedObject->globalAABB.x, aabb.x) + 1;
                v = collision_getIntersectionLen(сollidedObject->globalAABB.y, aabb.y) + 1;

                if (v > h) {
                    *left = h;
                } else if (h > v) {
                    // Если столкновение происходит по перпендикулярному направлению относительно движения персонажа, 
                    // коллизия определяется по оси движения объекта, с которым произошло столкновение
                    switch (сollidedObject->facingDirection) {
                        case DIRECTION_UP:
                            *bottom = h;
                            break;
                        case DIRECTION_DOWN:
                            *top = h;
                            break;
                    }
                }
            }
            break;
        // Двигаемся только вправо
        case DIRECTION_RIGHT:
            aabbRight = aabb_getRightAABB(aabb);
            // Проверяем столкновения в массиве коллизий
            if (collision_checkMapArea(aabbRight, &aabbRight)) {
                *right = aabb.x.max - aabbRight.x.min + 1;
            }
            // Проверяем столкновения с объектами
            if (сollidedObject != NULL && сollidedObject->globalAABB.x.min > aabb.x.min) {
                h = collision_getIntersectionLen(сollidedObject->globalAABB.x, aabb.x) + 1;
                v = collision_getIntersectionLen(сollidedObject->globalAABB.y, aabb.y) + 1;

                if (v > h) {
                    *right = h;
                } else if (h > v) {
                    // Если столкновение происходит по перпендикулярному направлению относительно движения персонажа, 
                    // коллизия определяется по оси движения объекта, с которым произошло столкновение
                    switch (сollidedObject->facingDirection) {
                        case DIRECTION_UP:
                            *bottom = h;
                            break;
                        case DIRECTION_DOWN:
                            *top = h;
                            break;
                    }
                }
            }
            break;
        // Двигаемся только вверх
        case DIRECTION_UP:
            aabbTop = aabb_getTopAABB(aabb);
            // Проверяем столкновения в массиве коллизий
            if (collision_checkMapArea(aabbTop, &aabbTop)) {
                *top = aabbTop.y.max - aabb.y.min + 1;
            }
            // Проверяем столкновения с объектами
            if (сollidedObject != NULL && сollidedObject->globalAABB.y.max < aabb.y.max) {
                h = collision_getIntersectionLen(сollidedObject->globalAABB.x, aabb.x) + 1;
                v = collision_getIntersectionLen(сollidedObject->globalAABB.y, aabb.y) + 1;

                if (h > v) {
                    // Если столкновение происходит по направлению движения персонажа, коллизия определяется по оси его движения
                    *top = v;
                } else if (v > h) {
                    // Если столкновение происходит по перпендикулярному направлению относительно движения персонажа, 
                    // коллизия определяется по оси движения объекта, с которым произошло столкновение
                    switch (сollidedObject->facingDirection) {
                        case DIRECTION_LEFT:
                            *right = h;
                            break;
                        case DIRECTION_RIGHT:
                            *left = h;
                            break;
                    }
                }
            }
            break;
        // Двигаемся только вниз
        case DIRECTION_DOWN:
            aabbBottom = aabb_getBottomAABB(aabb);
            // Проверяем столкновения в массиве коллизий
            if (collision_checkMapArea(aabbBottom, &aabbBottom)) {
                *bottom = aabb.y.max - aabbBottom.y.min + 1;
            }
            // Проверяем столкновения с объектами
            if (сollidedObject != NULL && сollidedObject->globalAABB.y.min > aabb.y.min) {
                h = collision_getIntersectionLen(сollidedObject->globalAABB.x, aabb.x) + 1;
                v = collision_getIntersectionLen(сollidedObject->globalAABB.y, aabb.y) + 1;

                if (h > v) {
                    *bottom = v;
                } else if (v > h) {
                    // Если столкновение происходит по перпендикулярному направлению относительно движения персонажа, 
                    // коллизия определяется по оси движения объекта, с которым произошло столкновение
                    switch (сollidedObject->facingDirection) {
                        case DIRECTION_LEFT:
                            *right = h;
                            break;
                        case DIRECTION_RIGHT:
                            *left = h;
                            break;
                    }
                }
            }
            break;
        // Двигаемся влево вверх
        case DIRECTION_LEFT_UP:
            // Проверяем столкновения в массиве коллизий
            aabbLeft = aabb_getLeftAABB(aabb);
            aabbTop = aabb_getTopAABB(aabb);

            if (collision_checkMapArea(aabbLeft, &aabbLeft)) {
                u8 h = collision_getIntersectionLen(aabbLeft.x, aabb.x) + 1;
                u8 v = collision_getIntersectionLen(aabbLeft.y, aabb.y) + 1;

                if (v > h) {
                    *left = h;
                } else if (v == h) {
                    *left = h;
                    *top = v;
                }
            }
            if (collision_checkMapArea(aabbTop, &aabbTop)) {
                u8 h = collision_getIntersectionLen(aabbTop.x, aabb.x) + 1;
                u8 v = collision_getIntersectionLen(aabbTop.y, aabb.y) + 1;

                if (h > v) {
                    *top = v;
                } else if (v == h) {
                    *left = h;
                    *top = v;
                }
            }
            // Проверяем столкновения с объектами
            if (сollidedObject != NULL && (сollidedObject->globalAABB.x.max < aabb.x.max || сollidedObject->globalAABB.y.max < aabb.y.max)) {
                h = collision_getIntersectionLen(сollidedObject->globalAABB.x, aabb.x) + 1;
                v = collision_getIntersectionLen(сollidedObject->globalAABB.y, aabb.y) + 1;

                if (v > h) {
                    *left = h;
                } else if (h > v) {
                    *top = v;
                } else if (v == h) {
                    *left = h;
                    *top = v;
                }
            }
            break;
        // Двигаемся вправо вверх
        case DIRECTION_RIGHT_UP:
            // Проверяем столкновения в массиве коллизий
            aabbRight = aabb_getRightAABB(aabb);
            aabbTop = aabb_getTopAABB(aabb);

            if (collision_checkMapArea(aabbRight, &aabbRight)) {
                u8 h = collision_getIntersectionLen(aabbRight.x, aabb.x) + 1;
                u8 v = collision_getIntersectionLen(aabbRight.y, aabb.y) + 1;

                if (v > h) {
                    *right = h;
                } if (v == h) {
                    *right = h;
                    *top = v;
                }
            }
            if (collision_checkMapArea(aabbTop, &aabbTop)) {
                u8 h = collision_getIntersectionLen(aabbTop.x, aabb.x) + 1;
                u8 v = collision_getIntersectionLen(aabbTop.y, aabb.y) + 1;

                if (h > v) {
                    *top = v;
                } if (v == h) {
                    *right = h;
                    *top = v;
                }
            }
            // Проверяем столкновения с объектами
            if (сollidedObject != NULL && (сollidedObject->globalAABB.x.min > aabb.x.min || сollidedObject->globalAABB.y.max < aabb.y.max)) {
                h = collision_getIntersectionLen(сollidedObject->globalAABB.x, aabb.x) + 1;
                v = collision_getIntersectionLen(сollidedObject->globalAABB.y, aabb.y) + 1;

                if (v > h) {
                    *right = h;
                } else if (h > v) {
                    *top = v;
                } else if (v == h) {
                    *right = h;
                    *top = v;
                }
            }
            break;
        // Двигаемся влево вниз
        case DIRECTION_LEFT_DOWN:
            // Проверяем столкновения в массиве коллизий
            aabbLeft = aabb_getLeftAABB(aabb);
            aabbBottom = aabb_getBottomAABB(aabb);

            if (collision_checkMapArea(aabbLeft, &aabbLeft)) {
                u8 h = collision_getIntersectionLen(aabbLeft.x, aabb.x) + 1;
                u8 v = collision_getIntersectionLen(aabbLeft.y, aabb.y) + 1;
            
                if (v > h) {
                    *left = h;
                } else if (v == h) {
                    *left = h;
                    *bottom = v;
                }
            }
            if (collision_checkMapArea(aabbBottom, &aabbBottom)) {
                u8 h = collision_getIntersectionLen(aabbBottom.x, aabb.x) + 1;
                u8 v = collision_getIntersectionLen(aabbBottom.y, aabb.y) + 1;
            
                if (h > v) {
                    *bottom = v;
                } else if (v == h) {
                    *left = h;
                    *bottom = v;
                }
            }
            // Проверяем столкновения с объектами
            if (сollidedObject != NULL && (сollidedObject->globalAABB.x.max < aabb.x.max || сollidedObject->globalAABB.y.min > aabb.y.min)) {
                h = collision_getIntersectionLen(сollidedObject->globalAABB.x, aabb.x) + 1;
                v = collision_getIntersectionLen(сollidedObject->globalAABB.y, aabb.y) + 1;
            
                if (v > h) {
                    *left = h;
                } else if (h > v) {
                    *bottom = v;
                } else if (v == h) {
                    *left = h;
                    *bottom = v;
                }
            }
            break;
        // Двигаемся вправо вниз
        case DIRECTION_RIGHT_DOWN:
            // Проверяем столкновения в массиве коллизий
            aabbRight = aabb_getRightAABB(aabb);
            aabbBottom = aabb_getBottomAABB(aabb);

            if (collision_checkMapArea(aabbRight, &aabbRight)) {
                u8 h = collision_getIntersectionLen(aabbRight.x, aabb.x) + 1;
                u8 v = collision_getIntersectionLen(aabbRight.y, aabb.y) + 1;

                if (v > h) {
                    *right = h;
                } else if (v == h) {
                    *right = h;
                    *bottom = v;
                }
            }
            if (collision_checkMapArea(aabbBottom, &aabbBottom)) {
                u8 h = collision_getIntersectionLen(aabbBottom.x, aabb.x) + 1;
                u8 v = collision_getIntersectionLen(aabbBottom.y, aabb.y) + 1;

                if (h > v) {
                    *bottom = v;
                } else if (v == h) {
                    *right = h;
                    *bottom = v;
                }
            }
            // Проверяем столкновения с объектами
            if (сollidedObject != NULL && (сollidedObject->globalAABB.x.min > aabb.x.min || сollidedObject->globalAABB.y.min > aabb.y.min)) {
                h = collision_getIntersectionLen(сollidedObject->globalAABB.x, aabb.x) + 1;
                v = collision_getIntersectionLen(сollidedObject->globalAABB.y, aabb.y) + 1;

                if (v > h) {
                    *right = h;
                } else if (h > v) {
                    *bottom = v;
                } else if (v == h) {
                    *right = h;
                    *bottom = v;
                }
            }
    }
}

u8 collision_getIntersectionLen(AxisLine_u16 a, AxisLine_u16 b) {
    u16 start = a.min > b.min ? a.min : b.min; // Большее из минимальных значений
    u16 end = a.max < b.max ? a.max : b.max;   // Меньшее из максимальных значений

    u8 len = end - start; // Вычисляем длину пересечения
    return len > 0 ? len : 0; // Если длина пересечения отрицательная, значит, пересечения нет
}