#include "SDL2/SDL_net.h"

#include "client.h"
#include "client_init.h"

#include "../shared/common/common.h"
#include "../shared/common/debug.h"

#include "../shared/logic/player.h"
#include "../shared/logic/terrain.h"
#include "../shared/logic/projectile.h"
#include "../shared/logic/input.h"
#include "../shared/net/net_utils.h"
#include "cnet.h"
#include <stdlib.h>

#define SERVER_IP "192.168.15.11"
#define SERVER_PORT 5555

int main() {
    Debug_StartTimer();

    Game game;

    /* Loading the game */
    Client_Init(&game);
    Terrain_Init(&game.terrain, game.renderer);
    Terrain_LoadFromPNG(&game.terrain, game.renderer,
                        "assets/img/maptest_background.png",
                        "assets/img/maptest_foreground.png");
    ClientInit_Players(game.players);

    Debug_Info("Game initialized successfully!");

    Client_Load(&game);

    Debug_Info("Game loaded successfully!");

    /* Game loop */
    int done = FALSE;
    while (!done) {
        done = Client_Update(&game);
        Client_Render(&game);
        /* wait 1/60 seconds (assuming our calculations take ZERO time) */
        SDL_Delay(1000/60);     /* the unit of this is milliseconds */
    }

    /* Cleaning up everything and exiting */
    Client_Clean(&game, EXIT_SUCCESS);
    return EXIT_SUCCESS;
}

void Client_Init(Game *game) {
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, SERVER_IP, SERVER_PORT)) {
        Debug_Error("SDLNet_ResolveHost failed: %s", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }
    game->server_socket = SDLNet_TCP_Open(&ip);
    if (!game->server_socket) {
        Debug_Error("SDLNet_TCP_Open failed: %s", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }

    game->server_socket_set = SDLNet_AllocSocketSet(1);
    if (!game->server_socket) {
        Client_Clean(game, EXIT_FAILURE);
    }
    SDLNet_TCP_AddSocket(game->server_socket_set, game->server_socket);

    char ip_char[16];
    NetUtil_IPint32ToChar(ip.host, ip_char);
    Debug_Info("Connected to server with IP %s port %d", ip_char, ip.port);

    CNet_InitHandlers();

    game->my_player_id = -1;
    game->start = 0;

    while (game->my_player_id == -1 || !game->start) {
        int quit = CNet_RecvFromServer(game, 60);
        if (quit) {
            Client_Clean(game, EXIT_FAILURE);
        }
    }
    Debug_Info("Communicated with server successfully. We have player_id = %d", game->my_player_id);

    game->window = NULL;
    game->renderer = NULL;

    game->w = WORLD_WIDTH;
    game->h = WORLD_HEIGHT;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        Debug_Error("Error initializing SDL: %s", SDL_GetError());
        Client_Clean(game, EXIT_FAILURE);
    }

    game->window = SDL_CreateWindow(GAME_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                 game->w, game->h, SDL_WINDOW_RESIZABLE);
    if (!game->window) {
        Debug_Error("Error creating SDL windows: %s", SDL_GetError());
        Client_Clean(game, EXIT_FAILURE);
    }

    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED);
    if (!game->renderer) {
        Debug_Error("Error creating SDL renderer: %s", SDL_GetError());
        Client_Clean(game, EXIT_FAILURE);
    }

    SDL_RenderSetLogicalSize(game->renderer, game->w, game->h); // automatic resizes to any resolution
}

void Client_Load(Game *game) {
    game->time = 0;

    Input_InitKeys(&game->input);

    for (int i = 0; i < NUM_PLAYERS; i++) {
        int status = Player_Load(&game->players[i], game->renderer);
        if (status == FAILURE) {
            Client_Clean(game, EXIT_FAILURE);
        }
    }

    int status = Projectile_Load(&game->projectile_sys, game->renderer);
    if (status == FAILURE) {
        Client_Clean(game, EXIT_FAILURE);
    }
}

int Client_Update(Game *game) {
    game->time++;

    int quit_local = Input_SetEvents(&game->event, &game->input);
    memcpy(&game->players[game->my_player_id].input, &game->input, sizeof(Input));

    CNet_SendInputToServer(game);

    int quit_net = CNet_RecvFromServer(game, 0);

    for (int i = 0; i < 2; i++) {
        Player_Update(&game->players[i], game);
    }

    Projectile_Update(&game->projectile_sys, game);

    return quit_local || quit_net;
}

void Client_Render(Game *game) {
    /* black background */           /*  red green blue alpha */
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    /* clear the window */
    SDL_RenderClear(game->renderer);

    Terrain_Render(&game->terrain, game->renderer);

    /* draw the image to the window */
    for (int i = 0; i < 2; i++) {
        Player_Render(&game->players[i], game->renderer);
    }

    Projectile_Render(&game->projectile_sys, game->renderer);

    SDL_RenderPresent(game->renderer);
}

void Client_Clean(Game *game, int exit_code) {
    if (game->server_socket) SDLNet_TCP_Close(game->server_socket);
    if (game->server_socket_set) SDLNet_FreeSocketSet(game->server_socket_set);
    for (int i = 0; i < 2; i++) {
        Player_Clean(&game->players[i]);
    }
    Terrain_Clean(&game->terrain);
    if (game->renderer) SDL_DestroyRenderer(game->renderer);
    if (game->window) SDL_DestroyWindow(game->window);
    Debug_Info("Game cleaned successfully!");
    SDL_Quit();
    exit(exit_code);
}
