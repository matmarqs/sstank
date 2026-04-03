#include <stdio.h>
#include <time.h>

#include "modules/main.h"
#include "config.h"

/* DEFINITIONS */
void loadGame(Game *game);
int processEvents(Game *game);
void draw(Game *game);
void clean(Game *game);

void initGame(Game *game) {
    game->win   = NULL;
    game->rendr = NULL;

    /* initialize graphics and timer system */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    /* Creating the window centered on the screen */
    game->win = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED,
                                         SDL_WINDOWPOS_CENTERED,
                                         WIN_WIDTH,  WIN_HEIGHT, 0);
    /* In case there is a error creating the window */
    if (!game->win) {
        printf("Error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    /* Create Renderer, which sets up hardware acceleration */
    game->rendr = SDL_CreateRenderer(game->win, -1, SDL_RENDERER_ACCELERATED);
    /* In case the is error creating the renderer */
    if (!game->rendr) {
        printf("Error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(game->win);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
}

void loadGame(Game *game) {
    SDL_Surface *surface = IMG_Load("img/mc-idle.png");
    if (!surface) {
        puts("Cannot find img/mc-idle.png!");
        clean(game);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
    game->mc.frame[HERO_IDLE] = SDL_CreateTextureFromSurface(game->rendr, surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("img/mc-down.png");
    if (!surface) {
        puts("Cannot find img/mc-down.png!");
        clean(game);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
    game->mc.frame[HERO_DOWN] = NULL;
    game->mc.frame[HERO_DOWN] = SDL_CreateTextureFromSurface(game->rendr, surface);
    SDL_FreeSurface(surface);

    /* Get width and height of the hero sprite and adjust them */
    SDL_QueryTexture(game->mc.frame[HERO_IDLE], NULL, NULL, &game->mc.w, &game->mc.h);
    game->mc.w *= 2;
    game->mc.h *= 2;

    /* Variables of the MC */
    game->mc.x = (WIN_WIDTH  - game->mc.w) / 2.0;   /* start sprite in center of screen */
    game->mc.y = (WIN_HEIGHT - game->mc.h) / 2.0;
    game->mc.dx = game->mc.dy = 0;
    game->mc.name = HERO_NAME;
    game->mc.health = 100;
    game->mc.anim = HERO_IDLE;
    game->mc.facingLeft = TRUE;
    game->mc.dead = FALSE;

    /* keep track of which inputs are given */
    game->input.up = game->input.down = game->input.left = game->input.right = FALSE;

    /* Variables of the Game */
    game->time = 0;
    game->state = 0;
    game->scrollX = game->scrollY = 0;
}

int processEvents(Game *game) {
    game->time++;
    int done = FALSE;
    /* process game->event-> */
    while (SDL_PollEvent(&game->event)) {
        switch(game->event.type) {
            case SDL_QUIT:
                done = TRUE;
                break;
            /* SDL_KEYDOWN describes a key PRESS */
            /* then we set the corresponding direction to TRUE */
            case SDL_KEYDOWN:
                switch (game->event.key.keysym.scancode) {
                    case K_UP:
                        game->input.up = TRUE;
                        break;
                    case K_LEFT:
                        game->input.left = TRUE;
                        break;
                    case K_DOWN:
                        game->input.down = TRUE;
                        break;
                    case K_RIGHT:
                        game->input.right = TRUE;
                        break;
                    default:
                        break;
                }
                break;
            /* SDL_KEYUP describes a key RELEASE */
            /* then we set the corresponding direction to FALSE */
            case SDL_KEYUP:
                switch (game->event.key.keysym.scancode) {
                    case K_UP:
                        game->input.up = FALSE;
                        break;
                    case K_LEFT:
                        game->input.left = FALSE;
                        break;
                    case K_DOWN:
                        game->input.down = FALSE;
                        break;
                    case K_RIGHT:
                        game->input.right = FALSE;
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

    /* determine velocity */
    game->mc.dx = game->mc.dy = 0;    /* 0 in cases where (up && down) or (left && right) */
    if (  game->input.up  && !game->input.down)  game->mc.dy = -SPEED;    /* negative goes up in the screen */
    if ( !game->input.up  &&  game->input.down)  game->mc.dy =  SPEED;
    if ( game->input.left && !game->input.right) game->mc.dx = -SPEED;
    if (!game->input.left &&  game->input.right) game->mc.dx =  SPEED;

    /* update positions */
    game->mc.x += game->mc.dx / 60;
    game->mc.y += game->mc.dy / 60;

    /* set the animation frame */
    game->mc.anim = (game->input.down) ? (HERO_DOWN) : (HERO_IDLE);

    /* collision detection with bounds */
    if (game->mc.x <= 0) game->mc.x = 0;
    if (game->mc.y <= 0) game->mc.y = 0;
    if (game->mc.x >= WIN_WIDTH  - game->mc.w) game->mc.x = WIN_WIDTH  - game->mc.w;
    if (game->mc.y >= WIN_HEIGHT - game->mc.h) game->mc.y = WIN_HEIGHT - game->mc.h;

    return done;
}

void draw(Game *game) {
    /* blue background */
                                    /*  red green blue alpha */
    SDL_SetRenderDrawColor(game->rendr, 128, 128, 255, 255);
    /* clear the window */
    SDL_RenderClear(game->rendr);

    /* draw the image to the window */
    SDL_Rect mcRect = { game->scrollX + game->mc.x,
                        game->scrollY + game->mc.y,
                        game->mc.w, game->mc.h };
    if (game->input.down) {
        SDL_RenderCopyEx(game->rendr, game->mc.frame[game->mc.anim],
                         NULL, &mcRect, 0, NULL, (game->time % 20 < 10));
    }
    else {
        SDL_RenderCopy(game->rendr, game->mc.frame[game->mc.anim],
                       NULL, &mcRect);
    }
    SDL_RenderPresent(game->rendr);
}

void clean(Game *game) {
    for (int i = 0; i < HERO_NUM_FRAMES; i++) {
        if (!game->mc.frame[i]) SDL_DestroyTexture(game->mc.frame[i]);
    }
    if (!game->rendr) SDL_DestroyRenderer(game->rendr);
    if (!game->win)   SDL_DestroyWindow(game->win);
}

int main() {
    Game game;

    /* Loading the game */
    initGame(&game);
    loadGame(&game);

    /* Game loop */
    int done = FALSE;
    while (!done) {
        done = processEvents(&game);
        draw(&game);
        /* wait 1/60 seconds (assuming our calculations take ZERO time) */
        SDL_Delay(1000/60);     /* the unit of this is milliseconds */
    }

    /* Cleaning up everything and exiting */
    clean(&game);
    SDL_Quit();
    return EXIT_SUCCESS;
}
