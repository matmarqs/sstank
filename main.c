#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_timer.h>
#include <stdlib.h>

#define TRUE  (1)
#define FALSE (0)

#define WIN_WIDTH  (1200)
#define WIN_HEIGHT (900)
#define SPEED      (300)   /* speed in pixels/sec */

#define BASE_PLAYER_HEIGHT  (100)

#define KEY_UP    (SDL_SCANCODE_UP)
#define KEY_LEFT  (SDL_SCANCODE_LEFT)
#define KEY_DOWN  (SDL_SCANCODE_DOWN)
#define KEY_RIGHT (SDL_SCANCODE_RIGHT)
#define KEY_ENTER (SDL_SCANCODE_KP_ENTER)

#define KEY_W (SDL_SCANCODE_W)
#define KEY_A (SDL_SCANCODE_A)
#define KEY_S (SDL_SCANCODE_S)
#define KEY_D (SDL_SCANCODE_D)
#define KEY_SPACE (SDL_SCANCODE_SPACE)

#define NUM_SPRITES (3)

typedef struct {
    float x, y;
    float vx;

    float w, h;
    float w_over_h;
    int facing_right;

    int curr_sprite;

    SDL_Texture *sprites[NUM_SPRITES];
} Player;

typedef struct {
    int w, a, s, d, space;
    int up, left, down, right, enter;
} Input;

typedef struct {
    SDL_Window *win;
    SDL_Renderer *renderer;

    SDL_Event event;

    Input input;

    Player players[2];

    int time;
} Game;

/* DEFINITIONS */
void init_game(Game *game);
void load_game(Game *game);
SDL_Texture *load_img_to_game(Game *game, char *img_path);
void clean_game(Game *game);

void init_game(Game *game) {
    game->win = NULL;
    game->renderer = NULL;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    game->win = SDL_CreateWindow("Projectile Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_WIDTH, WIN_HEIGHT, 0);
    if (!game->win) {
        printf("Error creating SDL windows: %s\n", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    game->renderer = SDL_CreateRenderer(game->win, -1, SDL_RENDERER_ACCELERATED);
    if (!game->renderer) {
        printf("Error creating SDL renderer: %s", SDL_GetError());
        SDL_DestroyWindow(game->win);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
}

SDL_Texture *load_img_to_game(Game *game, char *img_path) {
    SDL_Surface *surface = IMG_Load(img_path);
    if (!surface) {
        printf("load_surface_img error: cannot find %s\n", img_path);
        clean_game(game);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(game->renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void load_game(Game *game) {
    Player *players = game->players;

    players[0].sprites[0] = load_img_to_game(game, "assets/img/gato_maca0.png");
    players[0].sprites[1] = load_img_to_game(game, "assets/img/gato_maca1.png");
    players[0].sprites[2] = load_img_to_game(game, "assets/img/gato_maca2.png");
    players[0].w_over_h = 100.0 / 100.0;
    players[0].h = BASE_PLAYER_HEIGHT;
    players[0].w = BASE_PLAYER_HEIGHT * players[0].w_over_h;

    players[1].sprites[0] = load_img_to_game(game, "assets/img/gato_banana0.png");
    players[1].sprites[1] = load_img_to_game(game, "assets/img/gato_banana1.png");
    players[1].sprites[2] = load_img_to_game(game, "assets/img/gato_banana2.png");
    players[1].w_over_h = 637.0 / 358.0;
    players[1].h = BASE_PLAYER_HEIGHT;
    players[1].w = BASE_PLAYER_HEIGHT * players[1].w_over_h;

    players[0].x = (WIN_WIDTH - players[0].w) / 2.0;    /* start sprite center of screen */
    players[0].y = (WIN_HEIGHT - players[0].h) / 2.0;
    players[0].vx = 0;
    players[0].curr_sprite = 1;
    players[0].facing_right = TRUE;

    players[1].x = (WIN_WIDTH - players[1].w) / 2.0;
    players[1].y = (WIN_HEIGHT - players[1].h) / 2.0;
    players[1].vx = 0;
    players[1].curr_sprite = 1;
    players[1].facing_right = TRUE;

    game->input.up = game->input.left = game->input.down = game->input.right = game->input.enter = FALSE;
    game->input.w = game->input.a = game->input.s = game->input.d = game->input.space = FALSE;

    game->time = 0;
}

int process_events(Game *game) {
    game->time++;
    int done = FALSE;
    while (SDL_PollEvent(&game->event)) {
        switch(game->event.type) {
            case SDL_QUIT:
                done = TRUE;
                break;
            /* SDL_KEYDOWN describes a key PRESS */
            /* then we set the corresponding direction to TRUE */
            case SDL_KEYDOWN:
                switch (game->event.key.keysym.scancode) {
                    case KEY_UP:
                        game->input.up = TRUE;
                        break;
                    case KEY_LEFT:
                        game->input.left = TRUE;
                        break;
                    case KEY_DOWN:
                        game->input.down = TRUE;
                        break;
                    case KEY_RIGHT:
                        game->input.right = TRUE;
                        break;
                    case KEY_ENTER:
                        game->input.enter = TRUE;
                        break;
                    case KEY_W:
                        game->input.w = TRUE;
                        break;
                    case KEY_A:
                        game->input.a = TRUE;
                        break;
                    case KEY_S:
                        game->input.s = TRUE;
                        break;
                    case KEY_D:
                        game->input.d = TRUE;
                        break;
                    case KEY_SPACE:
                        game->input.space = TRUE;
                        break;
                    default:
                        break;
                }
                break;
            /* SDL_KEYUP describes a key RELEASE */
            /* then we set the corresponding direction to FALSE */
            case SDL_KEYUP:
                switch (game->event.key.keysym.scancode) {
                    case KEY_UP:
                        game->input.up = FALSE;
                        break;
                    case KEY_LEFT:
                        game->input.left = FALSE;
                        break;
                    case KEY_DOWN:
                        game->input.down = FALSE;
                        break;
                    case KEY_RIGHT:
                        game->input.right = FALSE;
                        break;
                    case KEY_ENTER:
                        game->input.enter = FALSE;
                        break;
                    case KEY_W:
                        game->input.s = FALSE;
                        break;
                    case KEY_A:
                        game->input.a = FALSE;
                        break;
                    case KEY_S:
                        game->input.s = FALSE;
                        break;
                    case KEY_D:
                        game->input.d = FALSE;
                        break;
                    case KEY_SPACE:
                        game->input.space = FALSE;
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

    Player *players = game->players;

    /* determine velocity */
    players[0].vx = 0;    /* 0 in cases where (left <- && right ->) */
    if ( game->input.left && !game->input.right) {
        players[0].vx = -SPEED;
        players[0].facing_right = FALSE;
    }
    if (!game->input.left &&  game->input.right) {
        players[0].vx =  SPEED;
        players[0].facing_right = TRUE;
    }

    players[1].vx = 0;    /* 0 in cases where (left A && right D) */
    if ( game->input.a && !game->input.d) {
        players[1].vx = -SPEED;
        players[1].facing_right = FALSE;
    }
    if (!game->input.a &&  game->input.d) {
        players[1].vx =  SPEED;
        players[1].facing_right = TRUE;
    }

    int t = game->time % 30;

    if (players[0].vx == 0) {
        players[0].curr_sprite = 1;
    }
    else {
        players[0].curr_sprite = (t < 10) ? 0 : (t < 20) ? 1 : 2;
    }

    if (players[1].vx == 0) {
        players[1].curr_sprite = 1;
    }
    else {
        players[1].curr_sprite = (t < 10) ? 0 : (t < 20) ? 1 : 2;
    }

    /* update positions */
    players[0].x += players[0].vx / 60;

    players[1].x += players[1].vx / 60;


    /* collision detection with bounds */
    if (players[0].x <= 0) players[0].x = 0;
    if (players[0].x >= WIN_WIDTH  - players[0].w) players[0].x = WIN_WIDTH  - players[0].w;

    if (players[1].x <= 0) players[1].x = 0;
    if (players[1].x >= WIN_WIDTH  - players[1].w) players[1].x = WIN_WIDTH  - players[1].w;

    return done;
}

void draw_game(Game *game) {
    /* blue background */
                                    /*  red green blue alpha */
    SDL_SetRenderDrawColor(game->renderer, 128, 128, 255, 255);
    /* clear the window */
    SDL_RenderClear(game->renderer);

    /* draw the image to the window */
    Player *players = game->players;
    SDL_Rect player0_rect = { players[0].x, players[0].y, players[0].w, players[0].h };
    SDL_Rect player1_rect = { players[1].x, players[1].y, players[1].w, players[1].h };

    SDL_RenderCopyEx(game->renderer, players[0].sprites[players[0].curr_sprite], NULL, &player0_rect, 0, NULL, players[0].facing_right);
    SDL_RenderCopyEx(game->renderer, players[1].sprites[players[1].curr_sprite], NULL, &player1_rect, 0, NULL, players[1].facing_right);

    SDL_RenderPresent(game->renderer);
}

void clean_game(Game *game) {
    for (int i = 0; i < NUM_SPRITES; i++) {
        if (game->players[0].sprites[i]) SDL_DestroyTexture(game->players[0].sprites[i]);
        if (game->players[1].sprites[i]) SDL_DestroyTexture(game->players[1].sprites[i]);
    }
    if (game->renderer) SDL_DestroyRenderer(game->renderer);
    if (game->win) SDL_DestroyWindow(game->win);
}

int main() {
    Game game;

    /* Loading the game */
    init_game(&game);
    load_game(&game);

    /* Game loop */
    int done = FALSE;
    while (!done) {
        done = process_events(&game);
        draw_game(&game);
        /* wait 1/60 seconds (assuming our calculations take ZERO time) */
        SDL_Delay(1000/60);     /* the unit of this is milliseconds */
    }

    /* Cleaning up everything and exiting */
    clean_game(&game);
    SDL_Quit();

    return EXIT_SUCCESS;
}
