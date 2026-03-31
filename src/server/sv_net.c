#include "sv_net.h"
#include <SDL2/SDL_net.h>

void Server_Broadcast(Server *server, uint8_t packet_id, void *data, int len) {
    Client *clients = server->clients;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) {
            NetProtocol_SendPacketToClient(&clients[i], packet_id, data, len);
        }
    }
}

void Server_InitNet(Server *server) {
    // Create socket
    if (SDLNet_Init() < 0) {
        Debug_Error("SDLNet_Init failed: %s", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, NULL, PORT) < 0) {
        Debug_Error("SDLNet_ResolveHost failed: %s", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }
    Debug_Info("Server listening on port %d", PORT);
    server->socket = SDLNet_TCP_Open(&ip);
    if (!server->socket) {
        Debug_Error("SDLNet_TCP_Open failed: %s", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }
    server->socket_set = SDLNet_AllocSocketSet(MAX_CLIENTS);
    if (!server->socket_set) {
        Debug_Error("SDLNet_AllocSocketSet failed: %s", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < MAX_CLIENTS; i++) {
        server->clients[i].socket = 0;
        server->clients[i].id = i;
        server->clients[i].active = 0;
    }
}

void Server_AcceptClients(Server *server) {
    while (1) {
        // Check for new connection
        TCPsocket client_socket = SDLNet_TCP_Accept(server->socket);
        Client *clients = server->clients;
        if (client_socket) {
            // Find free slot
            int slot = -1;
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (!clients[i].active) {
                    slot = i;
                    break;
                }
            }
            if (slot >= 0) {
                clients[slot].socket = client_socket;
                clients[slot].id = slot;
                clients[slot].active = 1;
                SDLNet_TCP_AddSocket(server->socket_set, client_socket);
                Debug_Info("Client %d connected", slot);
                // Send welcome with ID
                NetProtocol_SendPacketToClient(&clients[slot], PACKET_SV_WELCOME, &slot, 4);
            }
            else {
                // Server full
                NetProtocol_SendPacketToClient(&clients[slot], PACKET_SV_FULL, NULL, 0);
                SDLNet_TCP_Close(client_socket);
            }
        }
        int num_active_sockets = SDLNet_CheckSockets(server->socket_set, 50);
        char buffer[1024];
        if (num_active_sockets > 0) {
            // Check each client
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].active && SDLNet_SocketReady(clients[i].socket)) {
                    int bytes = SDLNet_TCP_Recv(clients[i].socket, buffer, sizeof(buffer));
                    if (bytes <= 0) {
                        // Client disconnected
                        Debug_Info("Client %d disconnected", i);
                        SDLNet_TCP_DelSocket(server->socket_set, clients[i].socket);
                        SDLNet_TCP_Close(clients[i].socket);
                        clients[i].active = 0;
                    }
                    else {
                        // Got data, but the other client is not ready yet.
                        NetProtocol_SendPacketToClient(&clients[i], PACKET_SV_WAITING, NULL, 0);
                    }
                }
            }
        }
        if (clients[0].active && clients[1].active) {
            Server_Broadcast(server, PACKET_SV_START, NULL, 0);
            Debug_Info("Both clients connected! Starting game!");
            break;
        }
        SDL_Delay(10);
    }
}
