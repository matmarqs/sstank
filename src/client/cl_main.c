#include "cl_main.h"
#include "cl_init.h"
#include "cl_input.h"
#include "cl_net.h"
#include "cl_player.h"
#include "cl_char.h"
#include "cl_terrain.h"
#include "cl_projectile.h"
#include "../shared/core_game.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        Debug_Info("Usage: ./client.elf SERVER_IP");
        return EXIT_FAILURE;
    }

    /* Loading the game */
    GameState game;
    Game_Init(&game);
    cl_state_t client;
    client.game = &game;
    Client_Init(&client, argv[1]);

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
    /* NETWORKING */
    ClientNet_InitSockets(client, ip_addr);
    ClientNet_InitHandlers();
    client->start = 0;
    client->my_player_id = -1;
    while (client->my_player_id == -1 || !client->start) {
        int quit = ClientNet_RecvFromServer(client, 60);
        if (quit) {
            Client_Clean(client, EXIT_FAILURE);
        }
    }
    Debug_Info("Connected. Player ID: %d", client->my_player_id);

    /* RENDERING */
    ClientInit_Rendering(client);

    /* GAME LOGIC */

    // Terrain
    client->cl_terrain.terrain = &client->game->terrain;
    ClientTerrain_Init(&client->cl_terrain, client->renderer);
    ClientTerrain_Load(&client->cl_terrain, MAP_BG, MAP_FG);

    // Players
    ClientInit_Players(client->cl_players);
    for (int i = 0; i < NUM_PLAYERS; i++) {
        client->cl_players[i].state = &client->game->players[i];
        ClientPlayer_Load(&client->cl_players[i], client->renderer);
    }
    ClientChar_Init(client);

    // Projectiles
    client->cl_projectile_sys.sys = &client->game->projectile_sys;
    ClientProjectile_Load(&client->cl_projectile_sys, client->renderer);
}

int Client_Update(cl_state_t *client) {
    GameState *game = client->game;
    game->time++;

    // Get input from SDL events
    int quit_local = Input_SetEvents(&client->event, &client->cl_char.input);

    ClientChar_Update(client);

    // Convert raw input to actions
    PlayerActions actions = {
        .move_left = client->cl_char.input.left,
        .move_right = client->cl_char.input.right,
    };

    // Send actions to server
    ClientNet_SendActions(client, actions);

    // Receive world state from server
    // Update local game state with server data
    // (Server updates happen via ClientNet_RecvFromServer)
    int quit_net = ClientNet_RecvFromServer(client, 0);

    // Run projectile physics locally (for rendering)
    ClientProjectile_Update(&client->cl_projectile_sys, client->game);

    // Update client-side rendering data
    for (int i = 0; i < NUM_PLAYERS; i++) {
        ClientPlayer_Update(&client->cl_players[i], game, actions, 1/60.0);
    }

    return quit_local || quit_net || client->game_over;
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
    ClientChar_RenderAngle(&client->cl_char, client->renderer);
    ClientChar_RenderPowerGauge(&client->cl_char, client->renderer);

    // Draw projectiles
    ClientProjectile_Render(&client->cl_projectile_sys, client->renderer);

    SDL_RenderPresent(client->renderer);
}

void Client_Clean(cl_state_t *client, int exit_code) {
    Game_Clean(client->game);

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
