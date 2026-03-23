#include "cl_net.h"

#include "../shared/net_protocol.h"
#include "cl_player.h"
#include "../shared/core_projectile.h"
#include "../shared/core_terrain.h"
#include "../shared/net_util.h"

static ClientNet_Handler handlers[256];

static int ClientNet_H_PACKET_SV_WELCOME(cl_state_t *client, void *data, int len_data) {
    UNUSED(len_data);
    int offset = sizeof(uint8_t);
    client->my_player_id = *(int *)(data + offset);
    return 0;
}

static int ClientNet_H_PACKET_SV_FULL(cl_state_t *client, void *data, int len_data) {
    UNUSED(client);
    UNUSED(data);
    UNUSED(len_data);
    return 1;
}

static int ClientNet_H_NOOP(cl_state_t *client, void *data, int len_data) {
    UNUSED(client);
    UNUSED(data);
    UNUSED(len_data);
    return 0;
}

static int ClientNet_H_PACKET_SV_START(cl_state_t *client, void *data, int len_data) {
    UNUSED(client);
    UNUSED(data);
    UNUSED(len_data);
    client->start = 1;
    return 0;
}

static int ClientNet_H_PACKET_SV_DISCONNECT(cl_state_t *client, void *data, int len_data) {
    UNUSED(client);
    UNUSED(data);
    UNUSED(len_data);
    return 1;   // return 1 in order to quit
}

static int ClientNet_H_PACKET_SV_MESSAGE(cl_state_t *client, void *data, int len_data) {
    UNUSED(len_data);
    GameState *game = client->game;
    int offset = sizeof(uint8_t);
    ServerMessage packet = *(ServerMessage *)(data + offset);
    uint8_t type = packet.type;
    switch (type) {
        case SVMSG_PLAYER_POS:
            game->players[packet.data.player_pos.id].x = packet.data.player_pos.x;
            game->players[packet.data.player_pos.id].y = packet.data.player_pos.y;
            break;
        case SVMSG_PLAYER_HEALTH:
            ClientPlayer_TakeDamage(&client->cl_players[packet.data.player_health.id],
                                    packet.data.player_health.health);
            break;
        case SVMSG_PROJECTILE_NEW:
            Projectile_Throw(&game->projectile_sys, packet.data.projectile_new.type,
                             packet.data.projectile_new.x, packet.data.projectile_new.y,
                             packet.data.projectile_new.angle, packet.data.projectile_new.power,
                             packet.data.projectile_new.id);
            break;
        case SVMSG_TERRAIN_DESTROY:
            Terrain_DestroyCircle(&game->terrain,
                                  packet.data.terrain_destroy.x, packet.data.terrain_destroy.y,
                                  packet.data.terrain_destroy.radius);
            client->cl_terrain.dirty = 1;
            break;
        case SVMSG_GAME_OVER:
            client->game_over = 1;
            Debug_Info("Game Over! Player %d wins!", packet.data.game_winner.winner);
            break;
        default:
            break;
    }
    return 0;
}

static int ClientNet_H_PACKET_SV_GAME_OVER(cl_state_t *client, void *data, int len_data) {
    UNUSED(len_data);
    int offset = sizeof(uint8_t);
    ServerMessage *msg = (ServerMessage *)(data + offset);
    Debug_Info("Game Over! Player %d wins!", msg->data.game_winner.winner);
    client->game_over = 1;
    return 0;
}

void ClientNet_InitHandlers() {
    handlers[PACKET_SV_WELCOME] = ClientNet_H_PACKET_SV_WELCOME;
    handlers[PACKET_SV_FULL] = ClientNet_H_PACKET_SV_FULL;
    handlers[PACKET_SV_WAITING] = ClientNet_H_NOOP;
    handlers[PACKET_SV_START] = ClientNet_H_PACKET_SV_START;
    handlers[PACKET_SV_DISCONNECT] = ClientNet_H_PACKET_SV_DISCONNECT;
    handlers[PACKET_SV_MESSAGE] = ClientNet_H_PACKET_SV_MESSAGE;
    handlers[PACKET_CL_MESSAGE] = ClientNet_H_NOOP;
    handlers[SVMSG_GAME_OVER] = ClientNet_H_PACKET_SV_GAME_OVER;
    for (int i = PACKET_FAKE_MAX; i < 256; i++) {
        handlers[i] = ClientNet_H_NOOP;
    }
}

int ClientNet_RecvFromServer(cl_state_t *game, int timeout) {
    int active_socket = SDLNet_CheckSockets(game->server_socket_set, timeout);
    if (active_socket) {
        char buffer[4096];
        int num_bytes_received = SDLNet_TCP_Recv(game->server_socket, buffer, sizeof(buffer));
        uint8_t packet_id = *(uint8_t *)buffer;
        int done = handlers[packet_id](game, buffer, num_bytes_received);
        return done;
    }
    return 0;
}

void ClientNet_InitSockets(cl_state_t *client, char *ip_addr) {
    // Networking init
    if (SDLNet_Init() < 0) {
        Debug_Error("SDLNet_Init failed: %s", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }

    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, ip_addr, SERVER_PORT)) {
        Debug_Error("SDLNet_ResolveHost failed: %s", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }

    client->server_socket = SDLNet_TCP_Open(&ip);
    if (!client->server_socket) {
        Debug_Error("SDLNet_TCP_Open failed: %s", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }

    client->server_socket_set = SDLNet_AllocSocketSet(1);
    if (!client->server_socket_set) {
        if (client->server_socket) SDLNet_TCP_Close(client->server_socket);
    }
    SDLNet_TCP_AddSocket(client->server_socket_set, client->server_socket);

    char ip_char[16];
    NetUtil_IPint32ToChar(ip.host, ip_char);
    Debug_Info("Connected to server with IP %s port %d", ip_char, ip.port);
}

void ClientNet_SendActions(cl_state_t *client, PlayerActions actions) {
    static uint32_t sequence = 0;

    ClientMessage cl_msg;
    cl_msg.type = CLMSG_PLAYER_INPUT;
    cl_msg.sequence = sequence++;
    cl_msg.timestamp = SDL_GetTicks();
    cl_msg.data.player_input.left = actions.move_left;
    cl_msg.data.player_input.right = actions.move_right;

    NetProtocol_SendPacketToServer(client->server_socket, PACKET_CL_MESSAGE, &cl_msg, sizeof(ClientMessage));
}

void ClientNet_SendThrow(cl_state_t *client, float angle, float power, int type) {
    static uint32_t sequence = 0;

    ClientMessage cl_msg;
    cl_msg.type = CLMSG_PLAYER_THROW;
    cl_msg.sequence = sequence++;
    cl_msg.timestamp = SDL_GetTicks();
    cl_msg.data.projectile_throw.angle = angle;
    cl_msg.data.projectile_throw.power = power;
    cl_msg.data.projectile_throw.type = type;

    NetProtocol_SendPacketToServer(client->server_socket, PACKET_CL_MESSAGE, &cl_msg, sizeof(ClientMessage));
}
