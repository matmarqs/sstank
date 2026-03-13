#include <SDL2/SDL_net.h>

#include "../shared/common/debug.h"

#define PORT 5555
#define MAX_CLIENTS 2

typedef struct {
    TCPsocket socket;
    int id;
    int active;
} Client;

static Client clients[MAX_CLIENTS];
static TCPsocket server_socket;
SDLNet_SocketSet socket_set;

void Server_Init() {
    // Create socket
    if (SDLNet_Init() < 0) {
        Debug_Error("SDLNet_Init failed: %s", SDLNet_GetError());
        exit(1);
    }

    IPaddress ip;

    if (SDLNet_ResolveHost(&ip, NULL, PORT) < 0) {
        Debug_Error("SDLNet_ResolveHost failed: %s", SDLNet_GetError());
        exit(1);
    }

    server_socket = SDLNet_TCP_Open(&ip);
    if (!server_socket) {
        Debug_Error("SDLNet_TCP_Open failed: %s", SDLNet_GetError());
        exit(1);
    }

    socket_set = SDLNet_AllocSocketSet(MAX_CLIENTS + 1);
    if (!socket_set) {
        Debug_Error("SDLNet_AllocSocketSet failed: %s", SDLNet_GetError());
        exit(1);
    }

    Debug_Info("Server listening on port %d", PORT);
}

void Server_AcceptClients() {
    while (1) {
        // Check for new connection
        TCPsocket client_socket = SDLNet_TCP_Accept(server_socket);

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

                SDLNet_TCP_AddSocket(socket_set, client_socket);

                Debug_Info("Client %d connected", slot);

                // Send welcome with ID
                char welcome[32];
                snprintf(welcome, sizeof(welcome), "ID:%d", slot);
                SDLNet_TCP_Send(client_socket, welcome, strlen(welcome));
            }
            else {
                // Server full
                char *msg = "FULL";
                SDLNet_TCP_Send(client_socket, msg, strlen(msg));
                SDLNet_TCP_Close(client_socket);
            }
        }

        if (clients[0].active && clients[1].active) {
            Debug_Info("Both clients connected! Starting game!");
            break;
        }

        SDL_Delay(10);
    }
}

void Server_Broadcast(const void *data, int len) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) {
            SDLNet_TCP_Send(clients[i].socket, data, len);
        }
    }
}

void Server_Loop() {
    char buffer[1024];

    while (1) {
        // Check for activity
        int active_sockets = SDLNet_CheckSockets(socket_set, 50);

        if (active_sockets > 0) {
            // Check each client
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].active && SDLNet_SocketReady(clients[i].socket)) {
                    int bytes = SDLNet_TCP_Recv(clients[i].socket, buffer, sizeof(buffer));

                    if (bytes <= 0) {
                        // Client disconnected
                        Debug_Info("Client %d disconnected", i);
                        SDLNet_TCP_DelSocket(socket_set, clients[i].socket);
                        SDLNet_TCP_Close(clients[i].socket);
                        clients[i].active = 0;
                    }
                    else {
                        // Got data! Forward to other client
                        buffer[bytes] = '\0';
                        Debug_Info("Client %d: %s", i, buffer);
                        int other = (i == 0) ? 1 : 0;
                        if (clients[other].active) {
                            SDLNet_TCP_Send(clients[other].socket, buffer, bytes);
                        }
                    }
                }
            }
        }
    }
}

void Server_Clean() {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) {
            SDLNet_TCP_Close(clients[i].socket);
        }
    }
    SDLNet_TCP_Close(server_socket);
    SDLNet_FreeSocketSet(socket_set);
    SDLNet_Quit();
}

int main() {
    Server_Init();
    Server_AcceptClients();
    Server_Loop();
    Server_Clean();
    return 0;
}
