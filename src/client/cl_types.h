#ifndef _CLIENT_TYPES_H
#define _CLIENT_TYPES_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

#include "../shared/core_types.h"

#define PLAYER_NUM_SPRITES 12
#define PROJECTILE_NUM_SPRITES 8

typedef struct {
    Terrain *terrain;
    // The combined result texture (streamed)
    SDL_Texture *render_texture;
    // For optimization: track if we need to rebuild
    int dirty;
    // Surface cache (to avoid repeated pixel reads)
    SDL_Surface *bg_surface;
    SDL_Surface *fg_surface;
} cl_terrain_t;

typedef struct {
    float angle;
    int facing_left;
} cl_projectile_control_t;

typedef struct {
    ProjectileSystem *sys;
    cl_projectile_control_t control[MAX_PROJECTILES];
    SDL_Texture *sprites[PROJECTILE_NUM_SPRITES];
} cl_projectile_sys_t;

typedef struct {
    int left, right;    // movement
    int up, down;   // angle aiming
    int shoot;  // shooting the projectile
    int change_arm; // change the weapon
} cl_input_t;

/* cl_player_t: (PlayerState + Rendering aspects) */
typedef struct {
    PlayerState *state;

    int damage_timer;
    float w_render, h_render;
    int sprite_inverted;
    int curr_sprite;

    char *sprites_path[PLAYER_NUM_SPRITES];
    SDL_Texture *sprites[PLAYER_NUM_SPRITES];
} cl_player_t;

/* cl_char_t: (PlayerState + Rendering aspects) + Inputs */
typedef struct {
    cl_player_t *player;
    int id;
    cl_input_t input;
    float angle;
    float angle_render;
    int throwing;
    int power;
    int change_arm_timer;
    int curr_arm;
    int projectile_timer;
    TCPsocket server;
} cl_char_t;

typedef struct {
    /* game logic */
    GameState *game;
    int game_over;
    /* client wrappers */
    cl_player_t cl_players[NUM_PLAYERS];
    cl_char_t cl_char;
    cl_projectile_sys_t cl_projectile_sys;
    cl_terrain_t cl_terrain;
    /* rendering */
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;
    /* network */
    int start;
    int my_player_id;
    TCPsocket server_socket;
    SDLNet_SocketSet server_socket_set;
} cl_state_t;

#endif
