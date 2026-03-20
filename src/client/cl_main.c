#include "cl_main.h"
#include "cl_init.h"
#include "cl_net.h"

#include "../shared/base_common.h"
#include "../shared/base_debug.h"

#include "../shared/logic_player.h"
#include "../shared/logic_terrain.h"
#include "../shared/logic_projectile.h"
#include "../shared/logic_input.h"
#include "../shared/net_util.h"

#define SERVER_PORT 5555

int main(int argc, char *argv[]) {
    if (argc < 2) {
        Debug_Info("Usage: ./client.elf SERVER_IP");
        return EXIT_FAILURE;
    }

    UNUSED(argc);
    Debug_StartTimer();

    ClientState client;
    GameState game = client.game;

    /* Loading the game */
    Client_Init(&client, argv[1]);
    Terrain_Init(&game.terrain, client.renderer);
    Terrain_LoadFromPNG(&game.terrain, client.renderer,
                        "assets/img/maptestruct_background.png",
                        "assets/img/maptestruct_foreground.png");
    ClientInit_Players(game.players);

    Client_Load(&client);

    /* game loop */
    int done = FALSE;
    while (!done) {
        done = Client_Update(&client);
        Client_Render(&client);
        /* wait 1/60 seconds (assuming our calculations take ZERO time) */
        SDL_Delay(1000/60);     /* the unit of this is milliseconds */
    }

    /* Cleaning up everything and exiting */
    Client_Clean(&client, EXIT_SUCCESS);
    return EXIT_SUCCESS;
}

void Client_Init(ClientState *client, char *ip_addr) {
    // Create socket
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
    if (!client->server_socket) {
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
    Debug_Info("Communicated with server successfully. We have player_id = %d", client->my_player_id);

    client->window = NULL;
    client->renderer = NULL;

    client->game.w = WORLD_WIDTH;
    client->game.h = WORLD_HEIGHT;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        Debug_Error("Error initializing SDL: %s", SDL_GetError());
        Client_Clean(client, EXIT_FAILURE);
    }

    client->window = SDL_CreateWindow(GAME_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                 1440, 900, SDL_WINDOW_RESIZABLE);
    if (!client->window) {
        Debug_Error("Error creating SDL windows: %s", SDL_GetError());
        Client_Clean(client, EXIT_FAILURE);
    }

    client->renderer = SDL_CreateRenderer(client->window, -1, SDL_RENDERER_ACCELERATED);
    if (!client->renderer) {
        Debug_Error("Error creating SDL renderer: %s", SDL_GetError());
        Client_Clean(client, EXIT_FAILURE);
    }

    SDL_RenderSetLogicalSize(client->renderer, client->game.w, client->game.h); // automatic resizes to any resolution
}

void Client_Load(ClientState *client) {
    client->game.time = 0;

    Input_InitKeys(&client->input);

    for (int i = 0; i < NUM_PLAYERS; i++) {
        int status = Player_Load(&client->game.players[i], client->renderer);
        if (status == FAILURE) {
            Client_Clean(client, EXIT_FAILURE);
        }
    }

    int status = Projectile_Load(&client->game.projectile_sys, client->renderer);
    if (status == FAILURE) {
        Client_Clean(client, EXIT_FAILURE);
    }
}

int Client_Update(ClientState *client) {
    GameState *game = &client->game;
    game->time++;

    int quit_local = Input_SetEvents(&client->event, &client->input);
    memcpy(&game->players[client->my_player_id].input, &client->input, sizeof(Input));

    ClientNet_SendInputToServer(client);

    int quit_net = ClientNet_RecvFromServer(client, 0);

    for (int i = 0; i < 2; i++) {
        Player_Update(&game->players[i], game);
    }

    Projectile_Update(&game->projectile_sys, game);

    return quit_local || quit_net;
}

void Client_Render(ClientState *client) {
    /* black background */           /*  red green blue alpha */
    SDL_SetRenderDrawColor(client->renderer, 0, 0, 0, 255);
    /* clear the window */
    SDL_RenderClear(client->renderer);

    Terrain_Render(&client->game.terrain, client->renderer);

    /* draw the image to the window */
    for (int i = 0; i < 2; i++) {
        Player_Render(&client->game.players[i], client->renderer);
    }

    Projectile_Render(&client->game.projectile_sys, client->renderer);

    SDL_RenderPresent(client->renderer);
}

void Client_Clean(ClientState *client, int exit_code) {
    if (client->server_socket) SDLNet_TCP_Close(client->server_socket);
    if (client->server_socket_set) SDLNet_FreeSocketSet(client->server_socket_set);
    for (int i = 0; i < 2; i++) {
        Player_Clean(&client->game.players[i]);
    }
    Terrain_Clean(&client->game.terrain);
    if (client->renderer) SDL_DestroyRenderer(client->renderer);
    if (client->window) SDL_DestroyWindow(client->window);
    SDLNet_Quit();
    Debug_Info("Client was cleaned successfully!");
    SDL_Quit();
    exit(exit_code);
}
