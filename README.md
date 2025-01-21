# Sega Mega Drive 2D Platformer

YouTube Video:

[![Preview](https://img.youtube.com/vi/HLaj8o_44Mg/0.jpg)](https://www.youtube.com/watch?v=HLaj8o_44Mg)

Представляю Вашему вниманию 2D платформер для игровой консоли Sega Mega Drive. Находится на ранней стадии разработки. Вот что уже реализовано:

- Главный персонаж и управление им  
- AABB и сопутствующие вспомогательные функции  
- `global.c`, содержащий изменяемые свойства для удобного управления игровыми уровнями  
- Игровые объекты и движок управления ими, включая глобальные указатели на функции обновления (`environment_onUpdateObject`, `environment_onUpdateObjectInViewport`, `environment_onObjectCollidesWithPlayerInViewport`), которые можно заменять в течение игры  
- Расчёт коллизий (на основе массива коллизий)  
- Скорость, ускорение и замедление (используется технология субпиксельного движения)  
- Coyote time  
- Скроллинг слоёв (background скроллится в зависимости от разницы размера с foreground)  
- Камера, включая естественное и принудительное перемещение  
- "Not scrollable" зона движения персонажа
- Различные события геймпада (press, release, hold)
- Движок стейджей. Реализованы следующие состояния:  
  - `STATE_INTRO` (начальные заставки)  
  - `STATE_START` (экран запуска игры и переход в меню)  
  - `STATE_GAME` (геймплей)  
  - `STATE_OPTIONS` (настройки)  
  - `STATE_CREDITS` (титры)  
- Код стейджей разделён на контроллер и сам стейдж (например, `controller_game.c` и `stage_game.c`)  
- Удобные инструменты отладки:  
  - Режим замедления игрового процесса  
  - Режим свободного перемещения  
  - Печать отладочной информации в консоль KDebug  
  - Печать отладочной информации на window plane игры  
  - Отображение FPS и загрузки CPU  
  - Прерывание игры с описанием ошибки на экране  
  - Отключение всех стейджей, предшествующих геймплею, для быстрого тестирования  
- Двигающиеся платформы, реализованные в виде игровых объектов, включая их коллизии (пока работают не идеально). На одном фрейме рассчитывается коллизия персонажа только с одним твёрдым игровым объектом (как двигающаяся платформа)
- Объекты-бонусы, которые можно подбирать  
- Игровой счёт  
- Анимации стояния и бега  
- Прочие мелочи  

### Управление на сегодня:
- **D-Pad**: движение  
- **B**: прыжок  
- **Z**: принудительно сдвинуть камеру влево  
- **C**: принудительно сдвинуть камеру вправо  
(*Z и C адаптированы под контроллер 8bitdo*)

### Дополнительная информация:
Для проекта требуется версия SGDK, которая находится в данный момент в репозитории на GitHub (январь 2025).  
В коде могут быть ужасные ошибки. Перед сборкой убедитесь, что Вам не нужно изменить значения констант в файле `debug.h`.  
На данный момент это НЕ игра.

Присоединяйтесь к моему Discord-серверу: https://discord.gg/EpYB2tHHfr

---

Let me introduce you to a 2D platformer for the Sega Mega Drive game console. It is currently in the early stages of development. Here’s what has already been implemented:

- The main character and its controls  
- AABB and related helper functions  
- `global.c`, containing adjustable properties for convenient management of game levels  
- Game objects and an engine to manage them, including global pointers to update functions (`environment_onUpdateObject`, `environment_onUpdateObjectInViewport`, `environment_onObjectCollidesWithPlayerInViewport`), which can be replaced during gameplay  
- Collision detection (based on a collision map)  
- Speed, acceleration, and deceleration (subpixel movement technology is used)  
- Coyote time  
- Layer scrolling (the background scrolls according to the size difference with the foreground)  
- Camera, including natural and forced movement  
- A "Not scrollable" zone for player movement  
- Various gamepad events (press, release, hold)  
- Stage engine. The following states have been implemented:  
  - `STATE_INTRO` (intro screens)  
  - `STATE_START` (game start screen and menu transitions)  
  - `STATE_GAME` (gameplay)  
  - `STATE_OPTIONS` (settings)  
  - `STATE_CREDITS` (credits)  
- Stage code is split into the controller and the stage itself (e.g., `controller_game.c` and `stage_game.c`)  
- Convenient debugging tools:  
  - Slow-motion mode for gameplay  
  - Free movement mode  
  - Printing debug information to the KDebug console  
  - Printing debug information to the game's window plane  
  - Displaying FPS and CPU load  
  - In-game interruptions with error descriptions on the screen  
  - Disabling all stages prior to the gameplay stage for quick testing  
- Moving platforms implemented as game objects, including collision detection with them (though it's not perfect yet). On each frame, collision is calculated with only one solid game object (like a moving platform).  
- Bonus objects that can be picked up  
- Game score tracking  
- Animations for standing and running  
- Other small details  

### Current controls:
- **D-Pad**: Move  
- **B**: Jump  
- **Z**: Force the camera to shift left  
- **C**: Force the camera to shift right  
(*Z and C are adapted for 8bitdo controllers*)

### Additional information:
The project requires the version of SGDK currently available in the GitHub repository (January 2025).  
The code may contain terrible bugs. Before building the game, make sure you don’t need to change constant values in the `debug.h` file.  
For now, this is NOT a game.

Join my Discord server:  https://discord.gg/EpYB2tHHfr
