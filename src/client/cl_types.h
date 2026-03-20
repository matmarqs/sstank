#ifndef _CLIENT_TYPES_H
#define _CLIENT_TYPES_H

#include <SDL2/SDL.h>

#include "../shared/struct_input_mapper.h"
#define PLAYER_NUM_SPRITES 12

#include "../shared/struct_game.h"
#include "../shared/struct_terrain.h"

#define PLAYER_NUM_SPRITES 12

typedef struct {
    int id;

    float x, y;
    float vx, vy;

    int on_ground;

    int alive;

    int throwing;
    int power;

    float health;

    int damage_timer;

    int change_arm_timer;
    int curr_arm;

    float w, h;
    float w_render, h_render;

    int facing_right;
    int sprite_inverted;

    float angle;
    float angle_render;

    int curr_sprite;

    Input input;

    InputMapper input_mapper;

    int projectile_timer;

    char *sprites_path[PLAYER_NUM_SPRITES];
    SDL_Texture *sprites[PLAYER_NUM_SPRITES];
} cl_player_t;

#define PROJECTILE_NUM_SPRITES  8

typedef struct {
    Projectile proj;
    float angle;
    int facing_left;
    int explosion_timer;
} cl_projectile_t;

typedef struct {
    cl_projectile_t projectiles[MAX_PROJECTILES];
    SDL_Texture *sprites[PROJECTILE_NUM_SPRITES];
    int count;
} cl_projectile_sys_t;

typedef struct {
    Terrain terrain;

    // The two source images
    SDL_Texture *background;  // Layer 1 - static beauty
    SDL_Texture *foreground;  // Layer 2 - destructible gameplay layer

    // The combined result texture (streamed)
    SDL_Texture *render_texture;

    // For optimization: track if we need to rebuild
    int dirty;

    // Surface cache (to avoid repeated pixel reads)
    SDL_Surface *bg_surface;
    SDL_Surface *fg_surface;
} cl_terrain_t;

typedef struct {
    GameState game;

    int start;

    int my_player_id;

    Input input;

    SDL_Window *window;
    SDL_Renderer *renderer;

    SDL_Event event;

    TCPsocket server_socket;
    SDLNet_SocketSet server_socket_set;
} ClientState;

#endif
