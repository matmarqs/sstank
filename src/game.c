#include "game.h"
#include "common.h"
#include "input.h"
#include "debug.h"

#include "game_init.c"
#include "projectile.h"
#include "terrain.h"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

int main() {
    Debug_StartTimer();

    Game game;

    /* Loading the game */
    Game_Init(&game);
    Terrain_Init(&game.terrain, game.renderer);
    Terrain_LoadFromPNG(&game.terrain, game.renderer,
                        "assets/img/maptest_background.png",
                        "assets/img/maptest_foreground.png");
    Init_Players(game.players);

    Debug_Info("Game initialized successfully!");

    Game_Load(&game);

    Debug_Info("Game loaded successfully!");

    /* Game loop */
    int done = FALSE;
    while (!done) {
        done = Game_Update(&game);
        Game_Draw(&game);
        /* wait 1/60 seconds (assuming our calculations take ZERO time) */
        SDL_Delay(1000/60);     /* the unit of this is milliseconds */
    }

    /* Cleaning up everything and exiting */
    Game_Clean(&game);
    Debug_Info("Game cleaned successfully!");

    SDL_Quit();
    return EXIT_SUCCESS;
}

void Game_Init(Game *game) {
    game->window = NULL;
    game->renderer = NULL;

    game->w = WORLD_WIDTH;
    game->h = WORLD_HEIGHT;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        Debug_Error("Error initializing SDL: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    game->window = SDL_CreateWindow(GAME_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                 game->w, game->h, SDL_WINDOW_RESIZABLE);
    if (!game->window) {
        Debug_Error("Error creating SDL windows: %s", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED);
    if (!game->renderer) {
        Debug_Error("Error creating SDL renderer: %s", SDL_GetError());
        SDL_DestroyWindow(game->window);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    SDL_RenderSetLogicalSize(game->renderer, game->w, game->h); // automatic resizes to any resolution
}

void Game_Load(Game *game) {
    game->time = 0;

    Input_InitKeys(&game->input);

    for (int i = 0; i < 2; i++) {
        int status = Player_Load(&game->players[i], game->renderer);
        if (status == FAILURE) {
            Game_Clean(game);
        }
    }

    int status = Projectile_Load(&game->projectile_sys, game->renderer);
    if (status == FAILURE) {
        Game_Clean(game);
    }
}

int Game_Update(Game *game) {
    game->time++;

    int done = Input_SetEvents(&game->event, &game->input);

    for (int i = 0; i < 2; i++) {
        Player_Update(&game->players[i], game);
    }

    Projectile_Update(&game->projectile_sys, game);

    return done;
}

void Game_Draw(Game *game) {
    ///* blue background */           /*  red green blue alpha */
    //SDL_SetRenderDrawColor(game->renderer, 128, 128, 255, 100);
    /* clear the window */
    SDL_RenderClear(game->renderer);

    Terrain_Render(&game->terrain, game->renderer);

    /* draw the image to the window */
    for (int i = 0; i < 2; i++) {
        Player_Draw(&game->players[i], game->renderer);
    }

    Projectile_Draw(&game->projectile_sys, game->renderer);

    SDL_RenderPresent(game->renderer);
}

void Game_Clean(Game *game) {
    for (int i = 0; i < 2; i++) {
        Player_Clean(&game->players[i]);
    }
    Terrain_Clean(&game->terrain);
    if (game->renderer) SDL_DestroyRenderer(game->renderer);
    if (game->window) SDL_DestroyWindow(game->window);
}
