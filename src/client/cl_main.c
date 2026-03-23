#include "cl_main.h"
#include "cl_init.h"
#include "cl_input.h"
#include "cl_net.h"
#include "cl_player.h"
#include "cl_char.h"
#include "cl_terrain.h"
#include "cl_projectile.h"
#include "../shared/core_game.h"
#include "cl_types.h"

int main(int argc, char *argv[]) {
    char *ip_addr = "127.0.0.1";
    if (argc < 2)
        Debug_Info("Assuming SERVER_IP is 127.0.0.1");
    else
        ip_addr = argv[1];

    /* Loading the game */
    GameState game;
    cl_state_t client;
    cl__Init(&client, &game, ip_addr);

    /* game loop */
    int done = FALSE;
    while (done) {
        done = cl__Update(&client);
        cl__Render(&client);
        SDL_Delay(1000/60);
    }

    cl__Clean(&client, EXIT_SUCCESS);
    return EXIT_SUCCESS;
}

void cl__Init(cl_state_t *client, GameState *game, char *ip_addr) {
    Game_Init(game);
    client->game = game;
    cl_init_Net(client, ip_addr);
    cl_init_Rendering(client);
    cl_init_Terrain(client);
    cl_init_Players(client);
    cl_init_Projectiles(client);
}

int cl__Update(cl_state_t *client) {
    GameState *game = client->game;
    game->time++;

    // Get input from SDL events
    int quit_local = cl_input_GetEvents(&client->event, &client->cl_char.input);

    cl_char_Update(&client->cl_char);

    // Receive world state from server
    // Update local game state with server data
    // (Server updates happen via cl_net_RecvFromServer)
    int quit_net = cl_net_RecvFromServer(client, 0);

    // Run projectile physics locally (for rendering)
    cl_projectile_Update(&client->cl_projectile_sys, client->game);

    // Update client-side rendering data
    for (int i = 0; i < NUM_PLAYERS; i++) {
        cl_player_Update(&client->cl_players[i], game, actions, 1/60.0);
    }

    return quit_local || quit_net || client->game_over;
}

void cl__Render(cl_state_t *client) {
    SDL_SetRenderDrawColor(client->renderer, 0, 0, 0, 255);
    SDL_RenderClear(client->renderer);

    // Draw terrain
    cl_terrain_Render(&client->cl_terrain, client->renderer);

    // Draw players
    for (int i = 0; i < NUM_PLAYERS; i++) {
        cl_player_Render(&client->cl_players[i], client->renderer);
    }

    // Draw power gauge for local player only
    cl_char_RenderAngle(&client->cl_char, client->renderer);
    cl_char_RenderPowerGauge(&client->cl_char, client->renderer);

    // Draw projectiles
    cl_projectile_Render(&client->cl_projectile_sys, client->renderer);

    SDL_RenderPresent(client->renderer);
}

void cl__Clean(cl_state_t *client, int exit_code) {
    cl_init_Clean(client, exit_code);
}
