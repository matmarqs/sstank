#include "base.h"
#include "net_protocol.h"

QUEUE_IMPLEMENT(sv_msg_queue_t, sv_msg, cl_msg_t, SV_MAX_MESSAGES);
QUEUE_IMPLEMENT(cl_msg_queue_t, cl_msg, sv_msg_t, CL_MAX_MESSAGES);

void net_SendPacketToClient(sv_client_t *client, uint8_t packet_id, void *data, int len_data) {
    if (client->active) {
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
        SDLNet_TCP_Send(client->socket, buffer, size_to_send);
        // debugging
        static int last_time;
        static int n_times = 0;
        int current_time = SDL_GetTicks();
        if (current_time - last_time > 1000) {
            n_times++;
            Debug_HexDump(buffer, size_to_send, "Send %d bytes to client %d", size_to_send, client->id);
            if (n_times >= 2) {
                last_time = current_time;
                n_times = 0;
            }
        }
    }
}

void net_SendPacketToServer(TCPsocket socket_to_send, uint8_t packet_id, void *data, int len_data) {
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
        Debug_HexDump(buffer, size_to_send, "Sent %d bytes to server", size_to_send);
        last_time = current_time;
    }
}
