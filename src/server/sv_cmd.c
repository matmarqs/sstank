#include <math.h>

#include "sv_cmd.h"
#include "sv_net.h"
#include "../shared/core_projectile.h"

int sv_cmd_PlayerShoot(Server *server, PlayerState *p, ProjectileSystem *ps,
                       int type, float angle, float power) {
    // Validate cooldown
    if (p->throw_cooldown > 0) {
        return -1;  // Still on cooldown
    }
    int center_x = p->x + p->w/2;
    int center_y = p->y + p->h/2;
    float dx = p->w/2;
    float dy = p->h/2;
    float radius = sqrt(dx*dx + dy*dy);
    float start_x = center_x + radius * cos(angle);
    float start_y = center_y + radius * (-sin(angle));
    int id = Projectile_Throw(ps, type, start_x, start_y, angle, power, p->id);
    if (0 <= id && id < MAX_PROJECTILES) {
        p->throw_cooldown = 30;  // 0.5 second cooldown at 60fps
        // Broadcast projectile to all clients
        ServerMessage sv_msg;
        sv_msg.type = SVMSG_PROJECTILE_NEW;
        sv_msg.data.projectile_new.owner_id = p->id;
        sv_msg.data.projectile_new.x = start_x;
        sv_msg.data.projectile_new.y = start_y;
        sv_msg.data.projectile_new.type = type;
        sv_msg.data.projectile_new.angle = angle;
        sv_msg.data.projectile_new.power = power;
        Server_Broadcast(server, PACKET_SV_MESSAGE, &sv_msg, sizeof(ServerMessage));
        Debug_Info("Player %d threw projectile type %d", p->id, type);
    }
    return -1;
}

void sv_cmd_PlayerTakeDamage(Server *server, int player_id, float health) {
  ServerMessage sv_msg;
  sv_msg.type = SVMSG_PLAYER_HEALTH;
  sv_msg.data.player_health.id = player_id;
  sv_msg.data.player_health.health = health;
  Server_Broadcast(server, PACKET_SV_MESSAGE, &sv_msg, sizeof(ServerMessage));
}

void sv_cmd_PlayerBroadcastPositions(Server *server) {
  if (server->game.time % 3 != 0)
    return;
  ServerMessage sv_msg;
  for (int i = 0; i < NUM_PLAYERS; i++) {
    PlayerState *p = &server->game.players[i];
    sv_msg.type = SVMSG_PLAYER_POS;
    sv_msg.data.player_pos.id = p->id;
    sv_msg.data.player_pos.x = p->x;
    sv_msg.data.player_pos.y = p->y;
    Server_Broadcast(server, PACKET_SV_MESSAGE, &sv_msg, sizeof(ServerMessage));
  } 
}

void sv_cmd_TerrainDestroy(Server *server, float cx, float cy, float r) {
  // Broadcast terrain destruction to clients
  ServerMessage sv_msg;
  sv_msg.type = SVMSG_TERRAIN_DESTROY;
  sv_msg.data.terrain_destroy.x = cx;
  sv_msg.data.terrain_destroy.y = cy;
  sv_msg.data.terrain_destroy.radius = r;
  Server_Broadcast(server, PACKET_SV_MESSAGE, &sv_msg, sizeof(ServerMessage));
}
