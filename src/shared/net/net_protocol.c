#include "net_protocol.h"
#include "../common/common.h"
#include "../common/debug.h"

static void NetProtocol_SendPacket(TCPsocket socket_to_send, uint8_t packet_id, void *data, int len_data, char *debug_string) {
    char buffer[sizeof(PacketID) + len_data];
    int offset = 0;
    buffer[offset] = packet_id;
    offset += sizeof(PacketID);
    int size_to_send = offset;
    if (data && len_data > 0) {
        int size_to_copy = MIN((int) (sizeof(buffer) - offset), len_data);
        memcpy(buffer + offset, data, size_to_copy);
        size_to_send = size_to_copy + offset;
    }
    SDLNet_TCP_Send(socket_to_send, buffer, size_to_send);
    Debug_HexDump(buffer, size_to_send, debug_string);
}

void NetProtocol_SendPacketToClient(Client *client, uint8_t packet_id, void *data, int len_data) {
    if (client->active) {
        char debug_string[100];
        snprintf(debug_string, sizeof(debug_string), "Sent data to client %d", client->id);
        NetProtocol_SendPacket(client->socket, packet_id, data, len_data, debug_string);
    }
}

void NetProtocol_SendPacketToServer(TCPsocket socket_to_send, uint8_t packet_id, void *data, int len_data) {
    NetProtocol_SendPacket(socket_to_send, packet_id, data, len_data, "Sent data to server");
}
