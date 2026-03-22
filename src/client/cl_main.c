// cl_main.c
#include "cl_main.h"
#include "cl_init.h"
#include "cl_input.h"
#include "cl_net.h"
#include "cl_player.h"
#include "cl_terrain.h"
#include "cl_projectile.h"
#include "../shared/net_util.h"

#define SERVER_PORT 5555

int main(int argc, char *argv[]) {
    if (argc < 2) {
        Debug_Info("Usage: ./client.elf SERVER_IP");
        return EXIT_FAILURE;
    }

    cl_state_t client;
    memset(&client, 0, sizeof(client));

    /* Loading the game */
    Client_Init(&client, argv[1]);

    // Terrain - use shared core functions
    ClientTerrain_Init(&client.cl_terrain, client.renderer);
    ClientTerrain_Load(&client.cl_terrain,
                       "assets/img/maptest_background.png",
                       "assets/img/maptest_foreground.png");

    // Players - use shared core functions
    for (int i = 0; i < NUM_PLAYERS; i++) {
        ClientPlayer_Load(&client.cl_players[i], client.renderer);
        client.cl_players[i].state = client.game.players[i];
    }
    ClientInit_Players(client.cl_players);

    // Projectiles
    ClientProjectile_Load(&client.cl_projectile_sys, client.renderer);

    Client_Load(&client);

    /* game loop */
    int done = FALSE;
    while (!done) {
        done = Client_Update(&client);
        Client_Render(&client);
        SDL_Delay(1000/60);
    }

    Client_Clean(&client, EXIT_SUCCESS);
    return EXIT_SUCCESS;
}

void Client_Init(cl_state_t *client, char *ip_addr) {
    // Networking init
    if (SDLNet_Init() < 0) {
        Debug_Error("SDLNet_Init failed: %s", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }

    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, ip_addr, SERVER_PORT)) {
        Debug_Error("SDLNet_ResolveHost failed: %s", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }

    client->server_socket = SDLNet_TCP_Open(&ip);
    if (!client->server_socket) {
        Debug_Error("SDLNet_TCP_Open failed: %s", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }

    client->server_socket_set = SDLNet_AllocSocketSet(1);
    if (!client->server_socket_set) {
        Client_Clean(client, EXIT_FAILURE);
    }
    SDLNet_TCP_AddSocket(client->server_socket_set, client->server_socket);

    char ip_char[16];
    NetUtil_IPint32ToChar(ip.host, ip_char);
    Debug_Info("Connected to server with IP %s port %d", ip_char, ip.port);

    ClientNet_InitHandlers();

    client->my_player_id = -1;
    client->start = 0;

    while (client->my_player_id == -1 || !client->start) {
        int quit = ClientNet_RecvFromServer(client, 60);
        if (quit) {
            Client_Clean(client, EXIT_FAILURE);
        }
    }
    Debug_Info("Connected. Player ID: %d", client->my_player_id);

    // SDL init
    client->game.w = WORLD_WIDTH;
    client->game.h = WORLD_HEIGHT;

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

    SDL_RenderSetLogicalSize(client->renderer, client->game.w, client->game.h);
}

void Client_Load(cl_state_t *client) {
    client->game.time = 0;

    // Copy player states from client struct to game struct
    for (int i = 0; i < NUM_PLAYERS; i++) {
        client->game.players[i] = client->cl_players[i].state;
    }
}

int Client_Update(cl_state_t *client) {
    GameState *game = &client->game;
    game->time++;

    // Get input from SDL events
    Input raw_input;
    Input_InitKeys(&raw_input);
    int quit_local = Input_SetEvents(&client->event, &raw_input);

    // Convert raw input to actions
    PlayerActions actions = {
        .move_left = raw_input.left,
        .move_right = raw_input.right,
    };

    // Send actions to server
    //ClientNet_SendActions(client, actions);

    // Receive world state from server
    int quit_net = ClientNet_RecvFromServer(client, 0);

    // Update local game state with server data
    // (Server updates happen via ClientNet_RecvFromServer)

    // Run projectile physics locally (for rendering)
    ClientProjectile_Update(&client->cl_projectile_sys, &client->game);

    // Update client-side rendering data
    for (int i = 0; i < NUM_PLAYERS; i++) {
        ClientPlayer_Update(&client->cl_players[i], game, actions, 1/60.0);
    }

    return quit_local || quit_net;
}

void Client_Render(cl_state_t *client) {
    SDL_SetRenderDrawColor(client->renderer, 0, 0, 0, 255);
    SDL_RenderClear(client->renderer);

    // Draw terrain
    ClientTerrain_Render(&client->cl_terrain, client->renderer);

    // Draw players
    for (int i = 0; i < NUM_PLAYERS; i++) {
        ClientPlayer_Render(&client->cl_players[i], client->renderer);
    }

    // Draw power gauge for local player only
    ClientPlayer_RenderPowerGauge(&client->cl_char, client->renderer);

    // Draw projectiles
    ClientProjectile_Render(&client->cl_projectile_sys, client->renderer);

    SDL_RenderPresent(client->renderer);
}

void Client_Clean(cl_state_t *client, int exit_code) {
    if (client->server_socket) SDLNet_TCP_Close(client->server_socket);
    if (client->server_socket_set) SDLNet_FreeSocketSet(client->server_socket_set);

    for (int i = 0; i < NUM_PLAYERS; i++) {
        ClientPlayer_Clean(&client->cl_players[i]);
    }

    ClientTerrain_Clean(&client->cl_terrain);
    ClientProjectile_Clean(&client->cl_projectile_sys);

    if (client->renderer) SDL_DestroyRenderer(client->renderer);
    if (client->window) SDL_DestroyWindow(client->window);

    SDLNet_Quit();
    Debug_Info("Client cleaned successfully");
    SDL_Quit();
    exit(exit_code);
}
