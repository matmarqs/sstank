#include <SDL2/SDL_image.h> // IMG_LoadTexture
#include <SDL2/SDL_render.h> // SDL_RenderCopyEx
#include <SDL2/SDL2_gfxPrimitives.h> // filledCircleRGBA
#include <math.h> // sin, cos

#include "../common/common.h"
#include "../common/debug.h"

#include "projectile.h"
#include "terrain.h" // Terrain_CheckCollision
#include "player.h" // Player_Teleport


int Projectile_Load(ProjectileSystem *ps, SDL_Renderer *renderer) {
    ps->sprites[0] = IMG_LoadTexture(renderer, "assets/img/toppng.com-bombs-1668x1686.png");
    if (!ps->sprites[0]) {
        Debug_Error("Bomb sprite not found");
        return FAILURE;
    }
    ps->sprites[1] = IMG_LoadTexture(renderer, "assets/img/airplane.png");
    if (!ps->sprites[1]) {
        Debug_Error("Airplane sprite not found");
        return FAILURE;
    }
    for (int i = 2; i < PROJECTILE_NUM_SPRITES; i++) {
        ps->sprites[i] = NULL;
    }

    for (int i = 0; i < MAX_PROJECTILES; i++) {
        ps->projectiles[i].state = PROJECTILE_INACTIVE;
    }
    ps->count = 0;

    return SUCCESS;
}

void Projectile_Throw(ProjectileSystem *ps, int type, float x, float y, float angle, float power, int owner) {
    if (ps->count >= MAX_PROJECTILES) {
        Debug_Warn("Maximum projectiles reached!");
        return;
    }

    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (ps->projectiles[i].state == PROJECTILE_INACTIVE) {
            float speed = power * 5.0f;
            ps->projectiles[i].type = type;
            ps->projectiles[i].x = x - PROJECTILE_WIDTH / 2.0;
            ps->projectiles[i].y = y - PROJECTILE_HEIGHT / 2.0;
            ps->projectiles[i].vx = speed * cos(angle);
            ps->projectiles[i].vy = -speed * sin(angle);    // y axis is inverted
            ps->projectiles[i].angle = 0;
            ps->projectiles[i].w = PROJECTILE_WIDTH;
            ps->projectiles[i].h = PROJECTILE_HEIGHT;
            ps->projectiles[i].state = PROJECTILE_ACTIVE;
            ps->projectiles[i].owner = owner;
            ps->projectiles[i].explosion_timer = 0;
            ps->projectiles[i].facing_left = (ps->projectiles[i].vx < 0) ? SDL_FLIP_VERTICAL : 0;
            return;
        }
    }
}

// Check if circle (explosion) intersects rectangle (player)
int CircleRectCollision(float circle_x, float circle_y, float radius,
                        float rect_x, float rect_y, float rect_w, float rect_h) {

    // Find closest point on rectangle to circle
    float closest_x = fmax(rect_x, fmin(circle_x, rect_x + rect_w));
    float closest_y = fmax(rect_y, fmin(circle_y, rect_y + rect_h));

    // Calculate distance from circle center to this closest point
    float dx = circle_x - closest_x;
    float dy = circle_y - closest_y;
    float dist_sq = dx*dx + dy*dy;

    return dist_sq <= radius * radius;
}

void Projectile_Update(ProjectileSystem *ps, Game *game) {
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
        p->x += p->vx / 60;
        p->y += p->vy / 60;
        p->vy += GRAVITY / 60;

        if (p->type == 0) {
            // Update rotation based on velocity
            if (fabs(p->vx) > 0.1f || fabs(p->vy) > 0.1f) {
                p->angle = atan2(-p->vy, fabs(p->vx));
            }
        }
        else if (p->type == 1) {
            // Base angle
            float base = -atan2(-p->vy, p->vx);

            // Bias depends on direction
            float bias = (p->vx > 0) ? -0.2f : 0.2f;

            p->angle = base + bias;
        }

        // Collision
        if (p->x < -p->w || p->x > game->w || p->y > game->h) {
            p->state = PROJECTILE_INACTIVE;
        }
        else if (Terrain_CheckCollision(&game->terrain, p->x, p->y, p->w, p->h)) {
            p->state = PROJECTILE_EXPLODING;
            p->explosion_timer = 10;
            float cx = p->x + p->w/2.0;
            float cy = p->y + p->h/2.0;
            if (p->type == 0) {
                Terrain_DestroyCircle(&game->terrain, cx, cy, BOMB_RADIUS);
                for (int i = 0; i < NUM_PLAYERS; i++) {
                    Player *player = &game->players[i];
                    if (!player->alive) continue;
                    if (CircleRectCollision(cx, cy, BOMB_RADIUS,
                                            player->x, player->y, 
                                            player->w, player->h)) {
                        player->health -= 20;
                        player->damage_timer = 60;
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
                        if (!Terrain_CheckCollision(&game->terrain, 
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

void Projectile_Render(ProjectileSystem *ps, SDL_Renderer *renderer) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        Projectile *p = &ps->projectiles[i];

        if (p->state == PROJECTILE_INACTIVE) continue;

        if (p->state == PROJECTILE_EXPLODING) {
            // Draw explosion
            int radius = BOMB_RADIUS + (10 - p->explosion_timer);
            if (p->type == 0) {
                filledCircleRGBA(renderer, p->x + p->w/2.0, p->y + p->h/2.0, radius, // reddish
                                 255, 200, 0, 100 + p->explosion_timer * 15);
            }
            else if (p->type == 1) {
                filledCircleRGBA(renderer, p->x + p->w/2.0, p->y + p->h/2.0, radius, // blueish
                                 50, 50, 255, 100 + p->explosion_timer * 15);
            }
            continue;
        }

        // Draw active projectile
        SDL_Rect rect = { p->x, p->y, p->w, p->h };
        if (p->type == 0) {
            SDL_RenderCopyEx(renderer, ps->sprites[p->type], NULL, &rect,
                             p->angle * 180 / CONST_PI, NULL, SDL_FLIP_NONE);
        }
        else if (p->type == 1) {
            SDL_RenderCopyEx(renderer, ps->sprites[p->type], NULL, &rect,
                             p->angle * 180 / CONST_PI, NULL, p->facing_left);
        }
    }
}

void Projectile_Clean(ProjectileSystem *ps) {
    for (int i = 0; i < PROJECTILE_NUM_SPRITES; i++) {
        if (ps->sprites[i]) {
            SDL_DestroyTexture(ps->sprites[i]);
            ps->sprites[i] = NULL;
        }
    }
}
