#include <SDL2/SDL_events.h>
#include <SDL2/SDL_net.h>
#include <stdint.h>
#include <arpa/inet.h> // ntohs
#define PORT 5555

#define MAX_CLIENTS 2
#define SPEED 300
#define WORLD_WIDTH 1440
#define WORLD_HEIGHT 900
#define PLAYER_WIDTH 60
#define PLAYER_HEIGHT 60

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define UNUSED(x) (void)(x)

enum {
    PACKET_SV_WELCOME,
    PACKET_SV_FULL,
    PACKET_SV_WAITING,
    PACKET_SV_START,
    PACKET_SV_DISCONNECT,
    PACKET_SV_MSG_PLAYER_POS,
    PACKET_CL_MSG_PLAYER_MOVE,
};

typedef struct {
    union {
        struct { uint8_t left, right; } move;
    } data;
} cl_msg_t;

typedef struct {
    union {
        struct { uint8_t id; float x, y; } pos;
    } data;
} sv_msg_t;

const int packet_sizes[256] = {
    [PACKET_SV_WELCOME] = sizeof(int), // 4 bytes (client id)
    [PACKET_SV_FULL] = 0,
    [PACKET_SV_WAITING] = 0,
    [PACKET_SV_START] = 0,
    [PACKET_SV_DISCONNECT] = 0,
    [PACKET_SV_MSG_PLAYER_POS] = sizeof(sv_msg_t),
    [PACKET_CL_MSG_PLAYER_MOVE] = sizeof(cl_msg_t),
};

void sh_send_packet(TCPsocket sock, uint8_t packet_id, void *data, int len_data) {
    char buffer[sizeof(uint8_t) + len_data];
    int offset = 0;
    buffer[offset] = packet_id;
    offset += sizeof(uint8_t);
    int size_to_send = offset;
    if (data && len_data > 0) {
        int size_to_copy = MIN((int) (sizeof(buffer) - offset), len_data);
        memcpy(buffer + offset, data, size_to_copy);
        size_to_send = size_to_copy + offset;
    }
    SDLNet_TCP_Send(sock, buffer, size_to_send);
}

typedef struct {
    int id;
    int w, h;
    float x, y;
} sh_player_t;

typedef struct {
    int time;
    sh_player_t players[MAX_CLIENTS];
} sh_game_t;


void sh_game_clean(sh_game_t *game) {
    UNUSED(game);
    return;
}

void sh_game_init(sh_game_t *game) {
    game->time = 0;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        game->players[i].id = i;
        int w = PLAYER_WIDTH;
        int h = PLAYER_HEIGHT;
        game->players[i].w = w;
        game->players[i].h = h;
        game->players[i].x = (WORLD_WIDTH - w) / 2.0;
        game->players[i].y = (WORLD_HEIGHT - h) / 2.0;
    }
}
