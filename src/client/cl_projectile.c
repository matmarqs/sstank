#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h> // filledCircleRGBA

#include "cl_projectile.h"
#include "../shared/core_projectile.h"

int ClientProjectile_Load(cl_projectile_sys_t *cl_ps, SDL_Renderer *renderer) {
    Projectile_Load(&cl_ps->sys);

    cl_ps->sprites[0] = IMG_LoadTexture(renderer, "assets/img/toppng.com-bombs-1668x1686.png");
    if (!cl_ps->sprites[0]) {
        Debug_Error("Bomb sprite not found");
        return FAILURE;
    }
    cl_ps->sprites[1] = IMG_LoadTexture(renderer, "assets/img/airplane.png");
    if (!cl_ps->sprites[1]) {
        Debug_Error("Airplane sprite not found");
        return FAILURE;
    }
    for (int i = 2; i < PROJECTILE_NUM_SPRITES; i++) {
        cl_ps->sprites[i] = NULL;
    }

    for (int i = 0; i < MAX_PROJECTILES; i++) {
        cl_ps->control[i].angle = 0;
        cl_ps->control[i].facing_left = 0;
    }

    return SUCCESS;
}

void ClientProjectile_Throw(cl_projectile_sys_t *ps, int type, float x, float y,
                            float angle, float power, int owner) {
    ProjectileSystem *sys = &ps->sys;
    int i = Projectile_Throw(sys, type, x, y, angle, power, owner);
    if (0 <= i && i < MAX_PROJECTILES) {
        Projectile *p = &sys->projectiles[i];
        cl_projectile_control_t *control = &ps->control[i];
        control->angle = 0;
        control->facing_left = (p->vx < 0) ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
    }
}

void ClientProjectile_Update(cl_projectile_sys_t *ps, GameState *game) {
    Projectile_Update(&ps->sys, game);

    /* Client only projectile control properties */
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        Projectile *p = &ps->sys.projectiles[i];
        if (p->state == PROJECTILE_INACTIVE) continue;

        cl_projectile_control_t *control = &ps->control[i];
        if (p->type == 0) {
            // Update rotation based on velocity
            if (fabs(p->vx) > 0.1f || fabs(p->vy) > 0.1f) {
                control->angle = atan2(-p->vy, fabs(p->vx));
            }
        }
        else if (p->type == 1) {
            // Base angle
            float base = -atan2(-p->vy, p->vx);
            // Bias depends on direction
            float bias = (p->vx > 0) ? -0.2f : 0.2f;
            control->angle = base + bias;
        }
    }
}

void ClientProjectile_Render(cl_projectile_sys_t *ps, SDL_Renderer *renderer) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        Projectile *p = &ps->sys.projectiles[i];
        cl_projectile_control_t *control = &ps->control[i];

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
                             control->angle * 180 / CONST_PI, NULL, SDL_FLIP_NONE);
        }
        else if (p->type == 1) {
            SDL_RenderCopyEx(renderer, ps->sprites[p->type], NULL, &rect,
                             control->angle * 180 / CONST_PI, NULL, control->facing_left);
        }
    }
}

void ClientProjectile_Clean(cl_projectile_sys_t *cl_ps) {
    for (int i = 0; i < PROJECTILE_NUM_SPRITES; i++) {
        if (cl_ps->sprites[i]) {
            SDL_DestroyTexture(cl_ps->sprites[i]);
            cl_ps->sprites[i] = NULL;
        }
    }
}
