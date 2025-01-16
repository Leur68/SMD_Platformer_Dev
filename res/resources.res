IMAGE sgdk "images/sgdk_logo.png" 0
IMAGE grass "images/grass.png" 0

SPRITE cursor_spr                    "sprites/cursor.bmp"            1 1 NONE 0 NONE NONE FAST TRUE
SPRITE player_sprite                 "sprites/player.png"            3 3 BEST 5 NONE NONE FAST TRUE
SPRITE smth_sprite                   "sprites/smth.png"              1 1 BEST 0 NONE NONE FAST TRUE
SPRITE m_sprite                      "sprites/moving_platform.png"   3 2 BEST 0 NONE NONE FAST TRUE

SPRITE tile_cursor_l_r               "images/tile_cursor_l_r.png"    1 1 FAST 5
SPRITE tile_cursor_r_r               "images/tile_cursor_r_r.png"    1 1 FAST 5
SPRITE tile_cursor_t_r               "images/tile_cursor_t_r.png"    1 1 FAST 5
SPRITE tile_cursor_b_r               "images/tile_cursor_b_r.png"    1 1 FAST 5

SPRITE player_cursor                 "images/player_cursor.png"      3 3 FAST 5

TILESET level1_tileset               "maps/level1.png"               FAST ALL
MAP     level1_map                   "maps/level1.png"               level1_tileset FAST 0
PALETTE level1_palette               "maps/level1.png"

TILESET level1_back_tileset          "maps/level1back.png"           FAST ALL
MAP     level1_back_map              "maps/level1back.png"           level1_back_tileset FAST 0
PALETTE level1_back_palette          "maps/level1back.png"