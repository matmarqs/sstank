#include <SDL2/SDL_net.h>

#include "../shared/common/debug.h"
#include "../shared/net/net_utils.h"

#define PORT 5555
#define MAX_CLIENTS 2

typedef struct {
    TCPsocket socket;
    int id;
    int active;
} Client;

typedef struct {
    Client clients[MAX_CLIENTS];
    TCPsocket socket;
    SDLNet_SocketSet socket_set;
} Server;

void Server_Init(Server *server) {
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

    char ip_char[16];
    NetUtil_IPint32ToChar(ip.host, ip_char);
    Debug_Info("IP: %s", ip_char);

    server->socket = SDLNet_TCP_Open(&ip);
    if (!server->socket) {
        Debug_Error("SDLNet_TCP_Open failed: %s", SDLNet_GetError());
        exit(1);
    }

    server->socket_set = SDLNet_AllocSocketSet(MAX_CLIENTS);
    if (!server->socket_set) {
        Debug_Error("SDLNet_AllocSocketSet failed: %s", SDLNet_GetError());
        exit(1);
    }

    for (int i = 0; i < MAX_CLIENTS; i++) {
        server->clients[i].socket = 0;
        server->clients[i].id = -1;
        server->clients[i].active = 0;
    }

    Debug_Info("Server listening on IP %s port %d", ip_char, PORT);
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
                char welcome[32];
                snprintf(welcome, sizeof(welcome), "ID: %d\n", slot);
                SDLNet_TCP_Send(client_socket, welcome, strlen(welcome));
            }
            else {
                // Server full
                char *msg = "Server FULL\n\0";
                SDLNet_TCP_Send(client_socket, msg, strlen(msg));
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
                        if (clients[i].active) {
                            char *msg = "Other player not ready yet\n\0";
                            SDLNet_TCP_Send(clients[i].socket, msg, strlen(msg));
                        }
                    }
                }
            }
        }

        if (clients[0].active && clients[1].active) {
            Debug_Info("Both clients connected! Starting game!");
            break;
        }

        SDL_Delay(10);
    }
}

void Server_Broadcast(Server *server, const void *data, int len) {
    Client *clients = server->clients;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) {
            SDLNet_TCP_Send(clients[i].socket, data, len);
        }
    }
}

void Server_Loop(Server *server) {
    char buffer[1024];
    Client *clients = server->clients;

    while (1) {
        // Check for activity
        int num_active_sockets = SDLNet_CheckSockets(server->socket_set, 50);

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

            if (!clients[0].active || !clients[1].active) {
                char *msg = "Other player disconnected, finalizing game\n\0";
                Server_Broadcast(server, msg, strlen(msg));
                Debug_Info(msg);
                break;
            }
        }
    }
}

void Server_Clean(Server *server) {
    Client *clients = server->clients;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) {
            SDLNet_TCP_Close(clients[i].socket);
        }
    }
    SDLNet_TCP_Close(server->socket);
    SDLNet_FreeSocketSet(server->socket_set);
    SDLNet_Quit();
}

int main() {
    Server server;
    Server_Init(&server);
    Server_AcceptClients(&server);
    Server_Loop(&server);
    Server_Clean(&server);
    return 0;
}
