#include "cl_init.h"
#include "cl_main.h"

void ClientInit_Player0(cl_player_t *p) {
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

void ClientInit_Player1(cl_player_t *p) {
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

void ClientInit_Rendering(cl_state_t *client) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        Debug_Error("SDL_Init failed: %s", SDL_GetError());
        Client_Clean(client, EXIT_FAILURE);
    }

    client->window = SDL_CreateWindow(GAME_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      1440, 900, SDL_WINDOW_RESIZABLE);
    if (!client->window) {
        Debug_Error("SDL_CreateWindow failed: %s", SDL_GetError());
        Client_Clean(client, EXIT_FAILURE);
    }

    client->renderer = SDL_CreateRenderer(client->window, -1, SDL_RENDERER_ACCELERATED);
    if (!client->renderer) {
        Debug_Error("SDL_CreateRenderer failed: %s", SDL_GetError());
        Client_Clean(client, EXIT_FAILURE);
    }

    SDL_RenderSetLogicalSize(client->renderer, WORLD_WIDTH, WORLD_HEIGHT);
}

void ClientInit_Players(cl_player_t p[]) {
    ClientInit_Player0(&p[0]);
    ClientInit_Player1(&p[1]);
}
