#include "../inc/global.h"

u8 collision_getTileIndex(u16 xTile, u16 yTile) {
    // Проверяем границы карты
    if (xTile < 0 || yTile < 0 || xTile >= mapWTiles || yTile >= mapHTiles) return true;
    // Возвращаем состояние тайла
    return masPointer2(collisionsMap, yTile, xTile);
}

bool collision_isTileSolid(u16 xTile, u16 yTile) {
    u8 index = collision_getTileIndex(xTile, yTile);
    return index == SOLID_TILE_INDEX || collision_checkMovingTileIndex(index, M_PLATFORM_TILE_INDEX);
}

bool collision_checkMovingTileIndex(u8 index, u8 middleIndex) {
    return index >= middleIndex - 8 && index <= middleIndex + 8;
}

s16 collision_getMovingTileShift(u16 xTile, u16 yTile, u8 middleIndex) {
    u8 index = collision_getTileIndex(xTile, yTile);
    if (!collision_checkMovingTileIndex(index, middleIndex)) {
        return 0;
    } else {
        return index - middleIndex;
    }
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

    

    /*
    AABB res;
    res.x.min = 0;
    res.y.min = 0;
    res.x.max = 0;
    res.y.max = 0;
    res.tileX.min = 0;
    res.tileY.min = 0;
    res.tileX.max = 0;
    res.tileY.max = 0;
    res.exists = false;
    bool start = true;
    for (s16 currYTile = aabb.tileY.min; currYTile < aabb.tileY.max; currYTile++) {
        for (s16 currXTile = aabb.tileX.min; currXTile < aabb.tileX.max; currXTile++) {
            if (collision_isTileSolid(collisions, currXTile, currYTile)) {
                if (start) {
                    res.tileX.min = currXTile;
                    res.tileY.min = currYTile;
                    res.x.min = res.tileX.min << 3;
                    res.y.min = res.tileY.min << 3;
                }
                res.tileX.max = currXTile;
                res.tileY.max = currYTile;
                res.x.max = res.tileX.max << 3;
                res.y.max = res.tileY.max << 3;
                // Если AABB шириной в один тайл, без корректировок ниже он будет шириной в 0 пикселей, что неверно
                if (res.y.max == res.y.min) {
                    res.y.max += 8;
                }
                if (res.x.max == res.x.min) {
                    res.x.max += 8;
                }
                start = false;
                res.exists = true;
            }
        }
    }
    return res;
    
    AABB res = {0};  // Инициализация всех полей нулями
    s16 xMin = aabb.tileX.max;   // Минимальные значения x и y тайлов
    s16 yMin = aabb.tileY.max;
    s16 xMax = aabb.tileX.min;   // Максимальные значения x и y тайлов
    s16 yMax = aabb.tileY.min;

    // Проходим по каждому тайлу в заданной области
    for (s16 currYTile = aabb.tileY.min; currYTile < aabb.tileY.max; currYTile++) {
        for (s16 currXTile = aabb.tileX.min; currXTile < aabb.tileX.max; currXTile++) {
            // Проверяем, является ли тайл твердым (коллизия)
            if (collision_isTileSolid(collisions, currXTile, currYTile)) {
                // Определяем минимальные и максимальные координаты тайлов
                if (currXTile < xMin) xMin = currXTile;
                if (currYTile < yMin) yMin = currYTile;
                if (currXTile > xMax) xMax = currXTile;
                if (currYTile > yMax) yMax = currYTile;
                
                res.exists = true;
            }
        }
    }

    // Если найдены коллизии, вычисляем AABB
    if (res.exists) {
        res.tileX.min = xMin;
        res.tileY.min = yMin;
        res.tileX.max = xMax;
        res.tileY.max = yMax;

        // Переводим координаты из тайлов в пиксели
        res.x.min = xMin << 3;  // Умножение на 8 с помощью сдвига
        res.y.min = yMin << 3;
        res.x.max = (xMax << 3) + 8;
        res.y.max = (yMax << 3) + 8;
    }

    return res;

    */
}

void collision_check(AABB aabb, u8 direction, u8* left, u8* right, u8* top, u8* bottom) {
    *left = *right = *top = *bottom = 0;

    // Получаем AABB для проверки столкновений, после этого отсеиваем среди найденных тайлов только те, что являются твердыми на карте. Данными тайлами считаются те, вплотную к которым или внутри которых находятся соответствующие стороны AABB персонажа
    AABB aabbLeft;
    AABB aabbTop;
    AABB aabbRight;
    AABB aabbBottom;
    
    /*
    s16 shift = 0;
    shift = collision_getMovingTileShift(aabbLeftObstacle.tileX.min, aabbLeftObstacle.tileY.min, M_PLATFORM_TILE_INDEX);
    if (shift != 0) {
        aabbLeftObstacle.x.min += shift;
        aabbLeftObstacle.x.max += shift;
    }

    shift = collision_getMovingTileShift(aabbRightObstacle.tileX.min, aabbRightObstacle.tileY.min, M_PLATFORM_TILE_INDEX);
    if (shift != 0) {
        aabbRightObstacle.x.min += shift;
        aabbRightObstacle.x.max += shift;
    }
    
    shift = collision_getMovingTileShift(aabbTopObstacle.tileX.min, aabbTopObstacle.tileY.min, M_PLATFORM_TILE_INDEX);
    if (shift != 0) {
        aabbTopObstacle.x.min += shift;
        aabbTopObstacle.x.max += shift;
    }
    
    shift = collision_getMovingTileShift(aabbBottomObstacle.tileX.min, aabbBottomObstacle.tileY.min, M_PLATFORM_TILE_INDEX);
    if (shift != 0) {
        aabbBottomObstacle.x.min += shift;
        aabbBottomObstacle.x.max += shift;
    }
    */
    
    // Рисуем тайлы с пересечениями для дебага
    #if (DEBUG_COLLISIONS)
        u8 tilesIR = 0;
        for (s16 x = aabbLeftObstacle.x.min; x < aabbLeftObstacle.x.max; x += 8) {    
            for (s16 y = aabbLeftObstacle.y.min; y < aabbLeftObstacle.y.max; y += 8) {
                SPR_setPosition(tileCursorsR[tilesIR++], x - cameraPosition.x, y - cameraPosition.y);
            }
        }
        for (u8 i = tilesIR; i < 3; i++) {
            SPR_setPosition(tileCursorsR[i], -8, -8);
        }
        tilesIR = 3;
        for (s16 x = aabbRightObstacle.x.min; x < aabbRightObstacle.x.max; x += 8) {    
            for (s16 y = aabbRightObstacle.y.min; y < aabbRightObstacle.y.max; y += 8) {
                SPR_setPosition(tileCursorsR[tilesIR++], x - cameraPosition.x, y - cameraPosition.y);
            }
        }
        for (u8 i = tilesIR; i < 6; i++) {
            SPR_setPosition(tileCursorsR[i], -8, -8);
        }
        tilesIR = 6;
        for (s16 x = aabbTopObstacle.x.min; x < aabbTopObstacle.x.max; x += 8) {    
            for (s16 y = aabbTopObstacle.y.min; y < aabbTopObstacle.y.max; y += 8) {
                SPR_setPosition(tileCursorsR[tilesIR++], x - cameraPosition.x, y - cameraPosition.y);
            }
        }
        for (u8 i = tilesIR; i < 9; i++) {
            SPR_setPosition(tileCursorsR[i], -8, -8);
        }
        tilesIR = 9;
        for (s16 x = aabbBottomObstacle.x.min; x < aabbBottomObstacle.x.max; x += 8) {    
            for (s16 y = aabbBottomObstacle.y.min; y < aabbBottomObstacle.y.max; y += 8) {
                SPR_setPosition(tileCursorsR[tilesIR++], x - cameraPosition.x, y - cameraPosition.y);
            }
        }
        for (u8 i = tilesIR; i < 12; i++) {
            SPR_setPosition(tileCursorsR[i], -8, -8);
        }
    #endif

    switch (direction) {
        // Двигаемся только влево
        case DIRECTION_LEFT: 
            aabbLeft = aabb_getLeftAABB(aabb);

            if (collision_checkMapArea(aabbLeft, &aabbLeft)) {
                *left = aabbLeft.x.max - aabb.x.min + 1;
            }
            break;
        // Двигаемся только вправо
        case DIRECTION_RIGHT:
            aabbRight = aabb_getRightAABB(aabb);

            if (collision_checkMapArea(aabbRight, &aabbRight)) {
                *right = aabb.x.max - aabbRight.x.min + 1;
            }
            break;
        // Двигаемся только вверх
        case DIRECTION_UP:
            aabbTop = aabb_getTopAABB(aabb);

            if (collision_checkMapArea(aabbTop, &aabbTop)) {
                *top = aabbTop.y.max - aabb.y.min + 1;
            }
            break;
        // Двигаемся только вниз
        case DIRECTION_DOWN:
            aabbBottom = aabb_getBottomAABB(aabb);

            if (collision_checkMapArea(aabbBottom, &aabbBottom)) {
                *bottom = aabb.y.max - aabbBottom.y.min + 1;
            }
            break;
        // Двигаемся влево вверх
        case DIRECTION_LEFT_UP:
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
            break;
        // Двигаемся вправо вверх
        case DIRECTION_RIGHT_UP:
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
            break;
        // Двигаемся влево вниз
        case DIRECTION_LEFT_DOWN:
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
            break;
        // Двигаемся вправо вниз
        case DIRECTION_RIGHT_DOWN:
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
    }
}

u8 collision_getIntersectionLen(AxisLine_u16 a, AxisLine_u16 b) {
    u16 start = a.min > b.min ? a.min : b.min; // Большее из минимальных значений
    u16 end = a.max < b.max ? a.max : b.max;   // Меньшее из максимальных значений

    u8 len = end - start; // Вычисляем длину пересечения
    return len > 0 ? len : 0; // Если длина пересечения отрицательная, значит, пересечения нет
}