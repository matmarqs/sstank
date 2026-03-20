#include <SDL2/SDL_image.h> // IMG_LoadTexture
#include <SDL2/SDL2_gfxPrimitives.h> // boxRGBA

#include "../shared/base_common.h"
#include "../shared/logic_player.h"

int loop_integer(int i, int n) {
    int period = 2 * (n - 1);  // For n=6, period = 10
    int t = i % period;
    if (t < n) {
        return t;           // 0,1,2,3,4,5
    } else {
        return period - t;  // 4,3,2,1
    }
}

void Player_Update(Player *p, GameState *game) {
    p->damage_timer--;
    p->damage_timer = MAX(p->damage_timer, 0);
    if (p->health <= 0) {
        p->alive = 0;
    }
    if (p->alive) {
        Player_ShootingHandler(p, game);
        Player_MovementHandler(p, game);
        Player_AnimationHandler(p, game);
    }
}

void Player_AnimationHandler(Player *p, GameState *game) {
    // Animation
    int t = game->time;
    if (p->vx == 0) {
        p->curr_sprite = loop_integer(t / 8, 5);
    } else {
        if (p->id == 0) {
            p->curr_sprite = 5 + loop_integer(t / 10, 7);
        }
        else {
            p->curr_sprite = 5 + loop_integer(t / 5, 7);
        }
    }
}

void Player_RenderPowerGauge(Player *p, SDL_Renderer *renderer) {
    int gauge_x = p->x - p->w/2;
    int gauge_y = p->y - p->w/2;  // Above the visor

    int gauge_width = 100;
    int gauge_height = 15;
    int border_size = 2;

    // Draw background (empty gauge)
    boxRGBA(renderer, 
            gauge_x, gauge_y, 
            gauge_x + gauge_width, gauge_y + gauge_height,
            50, 50, 50, 255);  // Dark gray

    // Draw border
    rectangleRGBA(renderer, 
                  gauge_x, gauge_y, 
                  gauge_x + gauge_width, gauge_y + gauge_height,
                  200, 200, 200, 255);

    // Draw fill based on power
    if (p->power > 0) {
        float normalized_power = (float)p->power / MAX_POWER;  // 0.0 to 1.0
        int fill_width = normalized_power * gauge_width;       // 0 to gauge_width

        // Color based on power level
        Uint8 r, g, b;
        float power_percent = normalized_power * 100;  // 0-100%

        if (power_percent < 30) {
            r = 0; g = 255; b = 0;    // Green
        } else if (power_percent < 70) {
            r = 255; g = 255; b = 0;  // Yellow
        } else {
            r = 255; g = 0; b = 0;    // Red
        }

        // Draw the filled portion (with border inset)
        boxRGBA(renderer, 
                gauge_x + border_size, 
                gauge_y + border_size,
                gauge_x + fill_width - border_size, 
                gauge_y + gauge_height - border_size,
                r, g, b, 255);
    }

    // If charging, show pulsing effect
    if (p->throwing) {
        int pulse = (SDL_GetTicks() / 100) % 2;
        if (pulse) {
            // White overlay
            boxRGBA(renderer,
                   gauge_x, gauge_y,
                   gauge_x + gauge_width, gauge_y + gauge_height,
                   255, 255, 255, 50);
        }
    }
}

void Player_Render(Player *p, SDL_Renderer *renderer) {
    if (p->alive) {
        if (p->damage_timer > 0) {
            // Make sprite reddish
            SDL_SetTextureColorMod(p->sprites[p->curr_sprite], 255, 100, 100);
            p->damage_timer--;
        } else {
            // Normal color
            SDL_SetTextureColorMod(p->sprites[p->curr_sprite], 255, 255, 255);
        }

        float x = p->x, y = p->y, w = p->w_render, h = p->h_render, theta = p->angle_render;

        SDL_Rect rect = { x, y, w, h };
        SDL_RenderCopyEx(renderer, p->sprites[p->curr_sprite], NULL, &rect, 0, NULL, p->sprite_inverted ? !p->facing_right : p->facing_right);

        int center_x = x + w/2;
        int center_y = y + h/2;
        float dx = x - center_x;
        float dy = y - center_y;
        float radius = sqrt(dx*dx + dy*dy);
        float norm = 50;

        float start_x = center_x + (radius) * cos(theta);
        float start_y = center_y + (radius) * (-sin(theta));   // y axis is inverted
        float end_x = center_x + (radius + norm) * cos(theta);
        float end_y = center_y + (radius + norm) * (-sin(theta));   // y axis is inverted

        //        // Draw UI elements
        Player_RenderPowerGauge(p, renderer);

        //circleRGBA(renderer, center_x, center_y, radius, 0, 0, 0, 255);

        //Debug_Info("theta = %f\n", theta * 360 / (2*CONST_PI));

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        thickLineRGBA(renderer, start_x, start_y, end_x, end_y, 5, 0, 0, 0, 255);

        // Reset color mod for other players (important!)
        SDL_SetTextureColorMod(p->sprites[p->curr_sprite], 255, 255, 255);
    }
}
