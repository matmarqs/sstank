#include <SDL2/SDL_image.h> // IMG_LoadTexture
#include <SDL2/SDL_render.h> // SDL_RenderCopyEx
#include <SDL2/SDL2_gfxPrimitives.h> // filledCircleRGBA
#include <math.h> // sin, cos

#include "core_projectile.h"
#include "core_player.h"
#include "core_terrain.h"
#include "core_physics.h"


int Projectile_Load(ProjectileSystem *ps) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        ps->projectiles[i].state = PROJECTILE_INACTIVE;
    }
    ps->count = 0;
    return SUCCESS;
}

int Projectile_Throw(ProjectileSystem *ps, int type, float x, float y, float angle, float power, int owner) {
    if (ps->count >= MAX_PROJECTILES) {
        Debug_Warn("Maximum projectiles reached!");
        return -1;
    }
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (ps->projectiles[i].state == PROJECTILE_INACTIVE) {
            float speed = power * 5.0f;
            ps->projectiles[i].type = type;
            ps->projectiles[i].x = x - PROJECTILE_WIDTH / 2.0;
            ps->projectiles[i].y = y - PROJECTILE_HEIGHT / 2.0;
            ps->projectiles[i].vx = speed * cos(angle);
            ps->projectiles[i].vy = -speed * sin(angle);    // y axis is inverted
            ps->projectiles[i].w = PROJECTILE_WIDTH;
            ps->projectiles[i].h = PROJECTILE_HEIGHT;
            ps->projectiles[i].state = PROJECTILE_ACTIVE;
            ps->projectiles[i].owner = owner;
            ps->projectiles[i].explosion_timer = 0;
            return i;
        }
    }
    return -1;
}


void Projectile_Update(ProjectileSystem *ps, GameState *game) {
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
                for (int i = 0; i < NUM_PLAYERS; i++) {
                    PlayerState *player = &game->players[i];
                    if (!player->alive) continue;
                    if (Physics_CircleRectCollision(cx, cy, BOMB_RADIUS,
                                            player->x, player->y, 
                                            player->w, player->h)) {
                        player->health -= 20;
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
