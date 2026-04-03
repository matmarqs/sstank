#include "shared.c"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <stdlib.h>

#define WINDOW_WIDTH 1440
#define WINDOW_HEIGHT 900
#define SERVER_IP "127.0.0.1"

typedef struct {
    int left, right;
} cl_input_t;

typedef struct {
    int id;
    TCPsocket server_socket;
    SDLNet_SocketSet sockset;
    SDL_Event event;
    SDL_Window *window;
    SDL_Renderer *renderer;
    cl_input_t input;
    sh_game_t *game;
    int game_running;
} cl_client_t;

void cl_clean(cl_client_t *client, int exit_code) {
    sh_game_clean(client->game);

    if (client->server_socket) SDLNet_TCP_Close(client->server_socket);
    if (client->sockset) SDLNet_FreeSocketSet(client->sockset);

    if (client->renderer) SDL_DestroyRenderer(client->renderer);
    if (client->window) SDL_DestroyWindow(client->window);

    SDLNet_Quit();
    SDL_Quit();

    printf("Client cleaned successfully\n");
    exit(exit_code);
}

typedef int (*cl_net_handler_t)(cl_client_t *, void *, int);
static cl_net_handler_t handlers[256];

int cl_net_recv(cl_client_t *client, int timeout) {
    static int last_time;
    int active_socket = SDLNet_CheckSockets(client->sockset, timeout);
    if (active_socket) {
        char buffer[4096];
        int num_bytes_received = SDLNet_TCP_Recv(client->server_socket, buffer, sizeof(buffer));
        if ((SDL_GetTicks() - last_time) > 1000) {
            printf("num_bytes_received = %d\n", num_bytes_received);
            last_time = SDL_GetTicks();
        }
        int offset = 0;
        while (offset < num_bytes_received) {
            uint8_t packet_id = *(uint8_t *)(buffer + offset);
            int done = handlers[packet_id](client, buffer + offset, 1 + packet_sizes[packet_id]);
            if (done) return 1;
            offset += 1 + packet_sizes[packet_id];
        }
    }
    return 0;
}

static int cl_net_H_NOOP(cl_client_t *client, void *data, int len_data) {
    UNUSED(client);
    UNUSED(data);
    UNUSED(len_data);
    return 0;
}

static int cl_net_H_PACKET_SV_WELCOME(cl_client_t *client, void *data, int len_data) {
    UNUSED(len_data);
    int offset = sizeof(uint8_t);
    client->id = *(int *)(data + offset);
    return 0;
}

static int cl_net_H_PACKET_SV_FULL(cl_client_t *client, void *data, int len_data) {
    UNUSED(client);
    UNUSED(data);
    UNUSED(len_data);
    return 1;
}

static int cl_net_H_PACKET_SV_WAITING(cl_client_t *client, void *data, int len_data) {
    return cl_net_H_NOOP(client, data, len_data);
}

static int cl_net_H_PACKET_SV_START(cl_client_t *client, void *data, int len_data) {
    client->game_running = 1;
    return cl_net_H_NOOP(client, data, len_data);
}

static int cl_net_H_PACKET_SV_DISCONNECT(cl_client_t *client, void *data, int len_data) {
    UNUSED(client);
    UNUSED(data);
    UNUSED(len_data);
    return 1;
}

static int cl_net_H_PACKET_SV_MSG_PLAYER_POS(cl_client_t *client, void *data, int len_data) {
    UNUSED(len_data);
    sh_game_t *game = client->game;
    int offset = sizeof(uint8_t);
    sv_msg_t packet = *(sv_msg_t *)(data + offset);
    game->players[packet.data.pos.id].x = packet.data.pos.x;
    game->players[packet.data.pos.id].y = packet.data.pos.y;
    return 0;
}

void cl_net_init_handlers() {
    for (int i = 0; i < 256; i++) {
        handlers[i] = cl_net_H_NOOP;
    }
    handlers[PACKET_SV_WELCOME] = cl_net_H_PACKET_SV_WELCOME;
    handlers[PACKET_SV_FULL] = cl_net_H_PACKET_SV_FULL;
    handlers[PACKET_SV_WAITING] = cl_net_H_PACKET_SV_WAITING;
    handlers[PACKET_SV_START] = cl_net_H_PACKET_SV_START;
    handlers[PACKET_SV_DISCONNECT] = cl_net_H_PACKET_SV_DISCONNECT;
    handlers[PACKET_SV_MSG_PLAYER_POS] = cl_net_H_PACKET_SV_MSG_PLAYER_POS;
}

void cl_net_init(cl_client_t *client, char *ip_addr) {
    if (SDLNet_Init() < 0) {
        printf("SDLNet_Init failed: %s\n", SDLNet_GetError());
        cl_clean(client, EXIT_FAILURE);
    }
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, ip_addr, PORT)) {
        printf("SDLNet_ResolveHost failed: %s\n", SDLNet_GetError());
        cl_clean(client, EXIT_FAILURE);
    }
    client->server_socket = SDLNet_TCP_Open(&ip);
    if (!client->server_socket) {
        printf("SDLNet_TCP_Open failed: %s\n", SDLNet_GetError());
        cl_clean(client, EXIT_FAILURE);
    }
    client->sockset = SDLNet_AllocSocketSet(1);
    if (!client->sockset) {
        cl_clean(client, EXIT_FAILURE);
    }
    SDLNet_TCP_AddSocket(client->sockset, client->server_socket);
    printf("Connected to server with IP %s port %d\n", ip_addr, (int) ntohs(ip.port));

    cl_net_init_handlers();

    client->id = -1;
    client->game_running = 0;
    while (client->id == -1 || !client->game_running) {
        int quit = cl_net_recv(client, 60);
        if (quit) {
            cl_clean(client, EXIT_FAILURE);
        }
    }
}

void cl_render_init(cl_client_t *client) {
    client->window = NULL;
    client->renderer = NULL;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        cl_clean(client, EXIT_FAILURE);
    }
    client->window = SDL_CreateWindow("Game",
                                      SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!client->window) {
        printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
        cl_clean(client, EXIT_FAILURE);
    }
    client->renderer = SDL_CreateRenderer(client->window, -1, SDL_RENDERER_ACCELERATED);
    if (!client->renderer) {
        printf("SDL_CreateRenderer failed: %s\n", SDL_GetError());
        cl_clean(client, EXIT_FAILURE);
    }
    SDL_RenderSetLogicalSize(client->renderer, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void cl_init(cl_client_t *client, sh_game_t *game) {
    client->game = game;
    sh_game_init(client->game);
    cl_net_init(client, SERVER_IP);
    cl_render_init(client);
}

int cl_input_get(SDL_Event *ev, cl_input_t *input) {
    int quit = 0;
    while (SDL_PollEvent(ev)) {
        switch(ev->type) {
            case SDL_QUIT:
                quit = 1;
                break;
            case SDL_KEYDOWN:
                switch(ev->key.keysym.scancode) {
                    case SDL_SCANCODE_A: input->left = 1; break;
                    case SDL_SCANCODE_D: input->right = 1; break;
                    default: break;
                }
                break;

            case SDL_KEYUP:
                switch(ev->key.keysym.scancode) {
                    case SDL_SCANCODE_A: input->left = 0; break;
                    case SDL_SCANCODE_D: input->right = 0; break;
                    default: break;
                }
                break;
            default:
                break;
        }
    }
    return quit;
}

void cl_input_send(cl_client_t *client) {
    cl_msg_t msg;
    msg.data.move.left = client->input.left;
    msg.data.move.right = client->input.right;
    sh_send_packet(client->server_socket, PACKET_CL_MSG_PLAYER_MOVE, &msg, sizeof(msg));
}

void cl_render(cl_client_t *client) {
    SDL_Renderer *rend = client->renderer;
    /* blue background */
                                    /*  red green blue alpha */
    SDL_SetRenderDrawColor(rend, 128, 128, 255, 255);
    /* clear the window */
    SDL_RenderClear(rend);

    /* draw the image to the window */
    for (int i = 0; i < MAX_CLIENTS; i++) {
        sh_player_t *p = &client->game->players[i];
        SDL_Rect rect = { p->x, p->y, p->w, p->h };
        SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
        SDL_RenderFillRect(rend, &rect);
    }
    SDL_RenderPresent(client->renderer);
}


void cl_loop(cl_client_t *client) {
    int done = 0;
    while (!done) {
        client->game->time++;
        int quit_local = cl_input_get(&client->event, &client->input);
        cl_input_send(client);
        int quit_net = cl_net_recv(client, 0);
        done = quit_local || quit_net;
        cl_render(client);
        SDL_Delay(1000/60.0);
    }
}

int main(int argc, char *argv[]) {
    UNUSED(argc);
    UNUSED(argv);
    cl_client_t client;
    sh_game_t game;
    cl_init(&client, &game);
    cl_loop(&client);
    cl_clean(&client, EXIT_SUCCESS);
    return 0;
}
