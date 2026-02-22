#include "game.h"
#include "common.h"
#include "input.h"
#include "debug.h"

#include "init.c"

int main() {
    Debug_StartTimer();

    Game game;

    /* Loading the game */
    Game_Init(&game);
    Init_Players(game.players);

    Debug_Info("Game initialized successfully!\n");

    Game_Load(&game);

    Debug_Info("Game loaded successfully!\n");

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
    Debug_Info("Game cleaned successfully!\n");

    SDL_Quit();
    return EXIT_SUCCESS;
}

void Game_Init(Game *game) {
    game->win = NULL;
    game->renderer = NULL;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        Debug_Error("Error initializing SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    game->win = SDL_CreateWindow(GAME_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                 WIN_WIDTH, WIN_HEIGHT, 0);
    if (!game->win) {
        Debug_Error("Error creating SDL windows: %s\n", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    game->renderer = SDL_CreateRenderer(game->win, -1, SDL_RENDERER_ACCELERATED);
    if (!game->renderer) {
        Debug_Error("Error creating SDL renderer: %s", SDL_GetError());
        SDL_DestroyWindow(game->win);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
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
}

int Game_Update(Game *game) {
    game->time++;

    int done = Input_SetEvents(&game->event, &game->input);

    for (int i = 0; i < 2; i++) {
        Player_Update(&game->players[i], game);
    }

    return done;
}

void Game_Draw(Game *game) {
    /* blue background */
                                    /*  red green blue alpha */
    SDL_SetRenderDrawColor(game->renderer, 128, 128, 255, 255);
    /* clear the window */
    SDL_RenderClear(game->renderer);

    /* draw the image to the window */
    for (int i = 0; i < 2; i++) {
        Player_Draw(&game->players[i], game->renderer);
    }

    SDL_RenderPresent(game->renderer);
}

void Game_Clean(Game *game) {
    for (int i = 0; i < 2; i++) {
        Player_Clean(&game->players[i]);
    }
    if (game->renderer) SDL_DestroyRenderer(game->renderer);
    if (game->win) SDL_DestroyWindow(game->win);
}
