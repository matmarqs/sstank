#include "core_game.h"
#include "core_terrain.h"
#include "core_player.h"
#include "core_projectile.h"

void Game_Init(GameState *game) {
    game->time = 0;
    game->w = WORLD_WIDTH;
    game->h = WORLD_HEIGHT;
    Terrain_Init(&game->terrain);
    Terrain_Load(&game->terrain, MAP_FG);
    for (int i = 0; i < NUM_PLAYERS; i++) {
        Player_Init(&game->players[i], i);
    }
    Projectile_Load(&game->projectile_sys);
}

void Game_Clean(GameState *game) {
    Terrain_Clean(&game->terrain);
}
