#include "cl_init.h"
#include "cl_net.h"
#include "cl_char.h"
#include "cl_player.h"
#include "cl_projectile.h"
#include "cl_terrain.h"
#include "../shared/core_game.h"

void cl_init_Net(cl_state_t *client, char *ip_addr) {
    cl_net_InitSockets(client, ip_addr);
    cl_net_InitHandlers();
    client->start = 0;
    client->my_player_id = -1;
    while (client->my_player_id == -1 || !client->start) {
        int quit = cl_net_RecvFromServer(client, 60);
        if (quit) {
            cl_init_Clean(client, EXIT_FAILURE);
        }
    }
    Debug_Info("Connected. Player ID: %d", client->my_player_id);
}

void cl_init_Rendering(cl_state_t *client) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        Debug_Error("SDL_Init failed: %s", SDL_GetError());
        cl_init_Clean(client, EXIT_FAILURE);
    }
    client->window = SDL_CreateWindow(GAME_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      1440, 900, SDL_WINDOW_RESIZABLE);
    if (!client->window) {
        Debug_Error("SDL_CreateWindow failed: %s", SDL_GetError());
        cl_init_Clean(client, EXIT_FAILURE);
    }
    client->renderer = SDL_CreateRenderer(client->window, -1, SDL_RENDERER_ACCELERATED);
    if (!client->renderer) {
        Debug_Error("SDL_CreateRenderer failed: %s", SDL_GetError());
        cl_init_Clean(client, EXIT_FAILURE);
    }
    SDL_RenderSetLogicalSize(client->renderer, WORLD_WIDTH, WORLD_HEIGHT);
}

void cl_init_SpritesPlayer0(cl_player_t *p) {
    p->sprite_inverted = TRUE;
    /* idle */
    p->sprites_path[ 0] = "assets/img/kirby_idle_0.png";
    p->sprites_path[ 1] = "assets/img/kirby_idle_0.png";
    p->sprites_path[ 2] = "assets/img/kirby_idle_0.png";
    p->sprites_path[ 3] = "assets/img/kirby_idle_1.png";
    p->sprites_path[ 4] = "assets/img/kirby_idle_1.png";
    /* running */
    p->sprites_path[ 5] = "assets/img/kirby_running_0.png";
    p->sprites_path[ 6] = "assets/img/kirby_running_2.png";
    p->sprites_path[ 7] = "assets/img/kirby_running_3.png";
    p->sprites_path[ 8] = "assets/img/kirby_running_4.png";
    p->sprites_path[ 9] = "assets/img/kirby_running_0.png";
    p->sprites_path[10] = "assets/img/kirby_running_2.png";
    p->sprites_path[11] = "assets/img/kirby_running_3.png";
}

void cl_init_SpritesPlayer1(cl_player_t *p) {
    p->sprite_inverted = TRUE;
    /* idle */
    p->sprites_path[ 0] = "assets/img/sonic_idle_0.png";
    p->sprites_path[ 1] = "assets/img/sonic_idle_1.png";
    p->sprites_path[ 2] = "assets/img/sonic_idle_2.png";
    p->sprites_path[ 3] = "assets/img/sonic_idle_3.png";
    p->sprites_path[ 4] = "assets/img/sonic_idle_4.png";
    /* running */
    p->sprites_path[ 5] = "assets/img/sonic_running_0.png";
    p->sprites_path[ 6] = "assets/img/sonic_running_1.png";
    p->sprites_path[ 7] = "assets/img/sonic_running_2.png";
    p->sprites_path[ 8] = "assets/img/sonic_running_3.png";
    p->sprites_path[ 9] = "assets/img/sonic_running_4.png";
    p->sprites_path[10] = "assets/img/sonic_running_5.png";
    p->sprites_path[11] = "assets/img/sonic_running_6.png";
}

void cl_init_SpritesPlayers(cl_player_t p[]) {
    cl_init_SpritesPlayer0(&p[0]);
    cl_init_SpritesPlayer1(&p[1]);
}

void cl_init_Players(cl_state_t *client) {
    // Players
    cl_init_SpritesPlayers(client->cl_players);
    for (int i = 0; i < NUM_PLAYERS; i++) {
        cl_player_Init(&client->cl_players[i], client->cl_players[i].state, client->renderer);
    }
    cl_char_Init(&client->cl_char, &client->cl_players[client->my_player_id], client->server_socket);
}

void cl_init_Terrain(cl_state_t *client) {
    cl_terrain_Init(&client->cl_terrain, &client->game->terrain, client->renderer);
    cl_terrain_Load(&client->cl_terrain, MAP_BG, MAP_FG);
}

void cl_init_Projectiles(cl_state_t *client) {
    cl_projectile_Load(&client->cl_projectile_sys, &client->game->projectile_sys, client->renderer);
}

void cl_init_Clean(cl_state_t *client, int exit_code) {
    Game_Clean(client->game);
    if (client->server_socket) SDLNet_TCP_Close(client->server_socket);
    if (client->server_socket_set) SDLNet_FreeSocketSet(client->server_socket_set);

    for (int i = 0; i < NUM_PLAYERS; i++) {
        cl_player_Clean(&client->cl_players[i]);
    }
    cl_terrain_Clean(&client->cl_terrain);
    cl_projectile_Clean(&client->cl_projectile_sys);
    if (client->renderer) SDL_DestroyRenderer(client->renderer);
    if (client->window) SDL_DestroyWindow(client->window);
    SDLNet_Quit();
    Debug_Info("Client cleaned successfully");
    SDL_Quit();
    exit(exit_code);
}
