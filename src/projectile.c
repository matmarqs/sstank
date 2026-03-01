#include "projectile.h"
#include "common.h"
#include "game.h"
#include "debug.h"
#include "terrain.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <math.h>

int Projectile_Load(ProjectileSystem *ps, SDL_Renderer *renderer) {
    ps->sprites[0] = IMG_LoadTexture(renderer, "assets/img/toppng.com-bombs-1668x1686.png");
    if (!ps->sprites[0]) {
        Debug_Error("Bomb sprite not found");
        return FAILURE;
    }
    for (int i = 1; i < PROJECTILE_NUM_SPRITES; i++) {
        ps->sprites[i] = NULL;
    }

    for (int i = 0; i < MAX_PROJECTILES; i++) {
        ps->projectiles[i].state = PROJECTILE_INACTIVE;
    }
    ps->count = 0;

    return SUCCESS;
}

void Projectile_Throw(ProjectileSystem *ps, float x, float y, float angle, float power, int owner) {
    if (ps->count >= MAX_PROJECTILES) {
        Debug_Warn("Maximum projectiles reached!");
        return;
    }

    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (ps->projectiles[i].state == PROJECTILE_INACTIVE) {
            float speed = power * 5.0f;
            ps->projectiles[i].x = x - PROJECTILE_WIDTH / 2.0;
            ps->projectiles[i].y = y - PROJECTILE_HEIGHT / 2.0;
            ps->projectiles[i].vx = speed * cos(angle);
            ps->projectiles[i].vy = -speed * sin(angle);    // y axis is inverted
            ps->projectiles[i].angle = angle;
            ps->projectiles[i].w = PROJECTILE_WIDTH;
            ps->projectiles[i].h = PROJECTILE_HEIGHT;
            ps->projectiles[i].curr_sprite = 0;
            ps->projectiles[i].state = PROJECTILE_ACTIVE;
            ps->projectiles[i].owner = owner;
            ps->projectiles[i].explosion_timer = 0;
            ps->projectiles[i].facing_right = ps->projectiles[i].vx > 0;
            return;
        }
    }
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

        // Update rotation based on velocity
        if (fabs(p->vx) > 0.1f || fabs(p->vy) > 0.1f) {
            p->angle = atan2(-p->vy, p->vx);
        }

        // Collision
        if (p->x < 0 || p->x > game->w || p->y < 0 || p->y > game->h
            || Terrain_CheckCollision(&game->terrain, p->x, p->y, p->w, p->h)) {
            p->state = PROJECTILE_EXPLODING;
            p->explosion_timer = 10;
            Terrain_DestroyCircle(&game->terrain, p->x + p->w/2.0, p->y + p->h/2.0, BOMB_RADIUS);
            Debug_Info("Projectile exploded!");
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
            filledCircleRGBA(renderer, p->x + p->w/2.0, p->y + p->h/2.0, radius, 
                           255, 200, 0, 100 + p->explosion_timer * 15);
            continue;
        }

        // Draw active projectile
        if (ps->sprites[0]) {
            SDL_Rect rect = { p->x, p->y, p->w, p->h };
            SDL_RenderCopyEx(renderer, ps->sprites[0], NULL, &rect,
                           p->angle * 180 / CONST_PI, NULL, SDL_FLIP_NONE);
        } else {
            filledCircleRGBA(renderer, p->x, p->y, BOMB_RADIUS, 
                           0, 0, 0, 255);
        }
    }
}

void Projectile_Clean(ProjectileSystem *ps) {
    for (int i = 0; i < 8; i++) {
        if (ps->sprites[i]) {
            SDL_DestroyTexture(ps->sprites[i]);
            ps->sprites[i] = NULL;
        }
    }
}
