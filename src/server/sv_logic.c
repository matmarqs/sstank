#include "sv_cmd.h"
#include "../shared/core_physics.h"
#include "../shared/core_terrain.h"
#include "../shared/core_player.h"

void sv_logic_ProjectileUpdate(sv_server_t *server) {
    GameState *game = &server->game;
    ProjectileSystem *ps = &game->projectile_sys;
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        Projectile *p = &ps->projectiles[i];
        if (p->state == PROJECTILE_INACTIVE) continue;
        if (p->state == PROJECTILE_EXPLODING) {
            p->explosion_timer--;
            if (p->explosion_timer <= 0) {
                p->state = PROJECTILE_INACTIVE;
                ps->count--;
            }
            continue;
        }
        // Physics update
        p->x += p->vx / 60.0;
        p->y += p->vy / 60.0;
        p->vy += GRAVITY / 60.0;
        // Collision
        if (p->x < -p->w || p->x > game->w || p->y > game->h) {
            p->state = PROJECTILE_INACTIVE;
        }
        else if (Physics_CheckCollision(&game->terrain, p->x, p->y, p->w, p->h)) {
            p->state = PROJECTILE_EXPLODING;
            p->explosion_timer = 10;
            float cx = p->x + p->w/2.0;
            float cy = p->y + p->h/2.0;
            if (p->type == 0) {
                Terrain_DestroyCircle(&game->terrain, cx, cy, BOMB_RADIUS);
                sv_cmd_TerrainDestroy(server, cx, cy, BOMB_RADIUS);
                for (int i = 0; i < NUM_PLAYERS; i++) {
                    PlayerState *player = &game->players[i];
                    if (!player->alive) continue;
                    if (Physics_CircleRectCollision(cx, cy, BOMB_RADIUS,
                                            player->x, player->y, 
                                            player->w, player->h)) {
                      player->health -= 20;
                      sv_cmd_PlayerTakeDamage(server, player->id, player->health);
                    }
                }
            }
            else if (p->type == 1) {
                // Search in a spiral pattern for safe spot
                int found = 0;
                for (int r = 0; r <= 50 && !found; r += 5) {
                    for (int angle = 0; angle < 360 && !found; angle += 30) {
                        float rad = angle * CONST_PI / 180.0;
                        float check_x = cx + r * cos(rad);
                        float check_y = cy + r * sin(rad);

                        // Center player at this position
                        float player_x = check_x - game->players[p->owner].w/2;
                        float player_y = check_y - game->players[p->owner].h/2;

                        // Check if position is safe
                        if (!Physics_CheckCollision(&game->terrain, 
                                                    player_x, player_y,
                                                    game->players[p->owner].w,
                                                    game->players[p->owner].h)) {
                            Player_Teleport(&game->players[p->owner], player_x, player_y);
                            found = 1;
                            break;
                        }
                    }
                }
            }
        }
    }
}
