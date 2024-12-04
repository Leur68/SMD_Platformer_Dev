#include "../inc/global.h"

Player* allocPlayer() {
    return (Player*) MEM_alloc(sizeof(Player));
}

void player_init(u16 startX, u16 startY) {
    // Sprite
    player->sprite = SPR_addSpriteSafe(&player_sprite, startX, startY, TILE_ATTR(PLAYER_PALETTE, 0, false, true));
    SPR_setAlwaysOnTop(player->sprite);
    PAL_setPalette(PLAYER_PALETTE, player_sprite.palette->data, DMA);
    
    // Position
    aabb_set(&player->globalAABB, (Vect2D_u16){ startX, startY + mapMaxCameraPosY });
    player->screenPos       = (Vect2D_u16){ startX, startY };
    player->posBuffer       = (Vect2D_ff32) { intToFastFix32(player->globalAABB.x.min), intToFastFix32(player->globalAABB.y.min) };

    // Moving
    player->velocity        = (Vect2D_ff32){ FASTFIX32(0), FASTFIX32(0) };
    player->autoVelocity    = (Vect2D_ff32){ FASTFIX32(0), FASTFIX32(0) };
    player->movedPixels     = (Vect2D_s16){ 0, 0 };

    player->facingDirection = DIRECTION_NONE;
    player->coyoteTimer     = 0;
    player->isMoving        = false;
    player->isAutoMoving    = false;
    player->isJumping       = false;
    player->isFalling       = false;
    player->decelerating    = false;

    // Collisions
    player->inUpperObstacle = false;
    player->inLowerObstacle = false;
    player->inLeftObstacle  = false;
    player->inRightObstacle = false;
}

// Инициализирует свойства игрока на текущем кадре
void player_update() {
    // Инициализируем начальное screenPos (до расчета коллизий) чтобы в купе с movedPixels это позволило camera_update() принять правильное решение о скроллинге
    // В player_update() не следует обновлять screenPos (после расчета коллизий и положения персонажа), так как это ответственность камеры
    player->screenPos.x = SPR_getPositionX(player->sprite);
    player->screenPos.y = SPR_getPositionY(player->sprite);

    // Обрабатываем замедление при отпускании клавиш движения
    // TODO сделать мгновенную смену направления движения в воздухе
    if (player->decelerating) {
        if (player->inLowerObstacle) {
            if (player->velocity.x < FASTFIX32(0)) { // left
                player->velocity.x += FASTFIX32(DECELERATION);
                if (player->velocity.x > FASTFIX32(0)) {
                    player->velocity.x = FASTFIX32(0);
                }
            } else if (player->velocity.x > FASTFIX32(0)) { // right
                player->velocity.x -= FASTFIX32(DECELERATION);
                if (player->velocity.x < FASTFIX32(0)) {
                    player->velocity.x = FASTFIX32(0);
                }
            }
            
            if (player->velocity.x == FASTFIX32(0)) {
                player->decelerating = false;
            }
        } else {
            player->decelerating = false;
            player->velocity.x = FASTFIX32(0);
        }
    }

    // Добавляем гравитацию
    #if (!DEBUG_COLLISIONS)
        if (!player->inLowerObstacle) {
            if (player->velocity.y < FASTFIX32(GRAVITY)) {
                player->velocity.y += FASTFIX32(GRAVITY_ACCELERATION);
            }
            if (player->velocity.y > FASTFIX32(GRAVITY)) {
                player->velocity.y = FASTFIX32(GRAVITY);
            }
        }
    #endif

    // Обновляем направление
    player->facingDirection = DIRECTION_NONE;
    if (player->velocity.x < FASTFIX32(0)) {
        player->facingDirection |= DIRECTION_LEFT;

        SPR_setHFlip(player->sprite, false); // Обновляем пространственную ориентацию спрайта
    } else if (player->velocity.x > FASTFIX32(0)) {
        player->facingDirection |= DIRECTION_RIGHT;

        SPR_setHFlip(player->sprite, true); // Обновляем пространственную ориентацию спрайта
    }
    if (player->velocity.y < FASTFIX32(0)) {
        player->facingDirection |= DIRECTION_UP;
    }
    if (!player->isJumping) {
        player->facingDirection |= DIRECTION_DOWN;
    }

    u8 lastLower = player->inLowerObstacle;

    // Обрабатываем коллизии и перемещаем персонажа
    player_move();

    if (player->inLowerObstacle && player->coyoteTimer != 0) {
        player->coyoteTimer = 0; // Обнуляем таймер
    } else if (lastLower && !player->inLowerObstacle) {
        player->coyoteTimer = 1; // Запускаем таймер если персонаж сорвался вниз
    } else if (player->coyoteTimer > 0 && player->coyoteTimer <= MAX_COYOTE_TIME) { // Позволяем таймеру выйти за максимальный предел на единицу
        player->coyoteTimer++;
    }

    // Эти значения следует рассчитывать именно здесь (после обработки коллизий)
    player->isMoving  = player->velocity.x != FASTFIX32(0) || player->velocity.y != FASTFIX32(0);
    player->isAutoMoving  = player->autoVelocity.x != FASTFIX32(0) || player->autoVelocity.y != FASTFIX32(0);
    player->isJumping = !player->inLowerObstacle && player->velocity.y < FASTFIX32(0);
    player->isFalling = !player->inLowerObstacle && player->velocity.y > FASTFIX32(0);

    // Обновляем анимации на основе состояния персонажа
    if (player->isFalling) {
        SPR_setAnim(player->sprite, ANIM_STAND);
    } else if (player->inLowerObstacle && player->isMoving) {
        SPR_setAnim(player->sprite, ANIM_WALK);
    } else {
        SPR_setAnim(player->sprite, ANIM_STAND);
    }
}

void player_move() {
    // Не проверяем коллизии если персонаж стоит на месте
    
    if (!player->isMoving && !player->isAutoMoving) {
        return;
    }

    // Вычисляем кол-во пикселей для перемещения персонажа в текущем фрейме

    player_calculateVelocity();

    if (player->movedPixels.x == 0 && player->movedPixels.y == 0) {
        return;
    }

    // Меняем координаты персонажа

    aabb_shift(&player->globalAABB, player->movedPixels);

    // Обрабатываем коллизии

    player_handleCollisions();

    #if (!DEBUG_COLLISIONS)
        // Останавливаем скорость по соответствующей оси при столкновении с препятствием
        if (player->inLeftObstacle || player->inRightObstacle) {
            player->velocity.x = FASTFIX32(0);
        }
        if (player->inUpperObstacle || player->inLowerObstacle) {
            player->velocity.y = FASTFIX32(0);
        }
    #endif
}

void player_calculateVelocity() {
    // Перемещаем персонажа субпикселями
    // На каждой итерации прибавляем скорость к "буферному" положению персонажа
    // Затем проверяем: если у буферного целая часть изменилась, то к нормальному положению прибавляем количество пикселей на которое изменилось буферное положение

    player->posBuffer.x += (player->velocity.x + player->autoVelocity.x);
    player->posBuffer.y += (player->velocity.y + player->autoVelocity.y);

    #if (DEBUG_INTERRUPT)
        if (player->posBuffer.x < FASTFIX32(0)) {
            SYS_die("player_calculateVelocity()", "posBuffer.x is negative", NULL);
        }
        if (player->posBuffer.y < FASTFIX32(0)) {
            SYS_die("player_calculateVelocity()", "posBuffer.y is negative", NULL);
        }
    #endif

    s16 newPosX = fastFix32ToInt(fastFix32Int(player->posBuffer.x));
    s16 newPosY = fastFix32ToInt(fastFix32Int(player->posBuffer.y));

    player->movedPixels.x = newPosX - player->globalAABB.x.min;
    player->movedPixels.y = newPosY - player->globalAABB.y.min;
}

void player_handleCollisions() {
    //if (player->facingDirection.y != DIRECTION_DOWN) {
    //    return;
    //}

    // Проверяем коллизии

    collision_check(player->globalAABB, player->facingDirection,
        &player->inLeftObstacle,
        &player->inRightObstacle,
        &player->inUpperObstacle,
        &player->inLowerObstacle);

    #if (!DEBUG_COLLISIONS)
        // Выталкиваем персонажа в нужную сторону если он застрял в препятствии (такое случается только при скорости больше 1)
        // Вычисляем величину выталкивания по двум осям

        Vect2D_s16 shift = {0, 0};
    
        if (player->inLeftObstacle > 1) {
            shift.x = player->inLeftObstacle - 1;
        } else if (player->inRightObstacle > 1) {
            shift.x = (1 - player->inRightObstacle);
        }
        if (player->inUpperObstacle > 1) {
            shift.y = player->inUpperObstacle - 1;
        } else if (player->inLowerObstacle > 1) {
            shift.y = (1 - player->inLowerObstacle);
        }

        if (shift.x != 0 || shift.y != 0) {
            // Выталкиваем
            aabb_shift(&player->globalAABB, shift);

            // Корректируем переменные
            player->posBuffer.x += FASTFIX32(shift.x);
            player->posBuffer.y += FASTFIX32(shift.y);
            player->movedPixels.x += shift.x;
            player->movedPixels.y += shift.y;

            // Заново рассчитываем коллизии
            collision_check(player->globalAABB, player->facingDirection,
                &player->inLeftObstacle,
                &player->inRightObstacle,
                &player->inUpperObstacle,
                &player->inLowerObstacle);
        }
    #endif
}