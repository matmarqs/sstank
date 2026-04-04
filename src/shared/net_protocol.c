#include "base.h"
#include "net_protocol.h"

static void net_SendPacket(TCPsocket socket_to_send, uint8_t packet_id, void *data, int len_data, char *debug_string) {
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
    SDLNet_TCP_Send(socket_to_send, buffer, size_to_send);
    // debugging
    static int last_time;
    int current_time = SDL_GetTicks();
    if (current_time - last_time > 1000) {
        Debug_HexDump(buffer, size_to_send, debug_string);
        last_time = current_time;
    }
}

void net_SendPacketToClient(sv_client_t *client, uint8_t packet_id, void *data, int len_data) {
    if (client->active) {
        char debug_string[100];
        snprintf(debug_string, sizeof(debug_string), "Sent data to client %d", client->id);
        net_SendPacket(client->socket, packet_id, data, len_data, debug_string);
    }
}

void net_SendPacketToServer(TCPsocket socket_to_send, uint8_t packet_id, void *data, int len_data) {
    net_SendPacket(socket_to_send, packet_id, data, len_data, "Sent data to server");
}
