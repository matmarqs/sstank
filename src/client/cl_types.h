#ifndef _CLIENT_TYPES_H
#define _CLIENT_TYPES_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

#include "../shared/core_types.h"

#define PLAYER_NUM_SPRITES 12
#define PROJECTILE_NUM_SPRITES 8

typedef struct {
    int w, a, s, d, space, l_shift;
} Input;

typedef struct {
    int (*move_left)(Input *input);
    int (*move_right)(Input *input);
    int (*increase_angle)(Input *input);
    int (*decrease_angle)(Input *input);
    int (*throw_projectile)(Input *input);
    int (*cycle_arm)(Input *input);
} InputMapper;

typedef struct {
    PlayerState state;

    int damage_timer;
    float w_render, h_render;
    int sprite_inverted;
    int curr_sprite;
    int projectile_timer;

    char *sprites_path[PLAYER_NUM_SPRITES];
    SDL_Texture *sprites[PLAYER_NUM_SPRITES];
} cl_player_t;

typedef struct {
    cl_player_t player;
    Input input;
    float angle;
    float angle_render;
    int throwing;
    int power;
    int change_arm_timer;
    int curr_arm;
} cl_char_t;

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
