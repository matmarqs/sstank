#include "../common/common.h"

#include "input.h"

void Input_InitKeys(Input *input) {
    input->up = input->left = input->down = input->right = input->space = input->l_shift = FALSE;
    input->w = input->a = input->s = input->d = input->enter = input->r_ctrl = FALSE;
}

int Input_SetEvents(SDL_Event *ev, Input *input) {
    int done = FALSE;

    while (SDL_PollEvent(ev)) {
        switch(ev->type) {
            case SDL_QUIT:
                done = TRUE;
                break;

            case SDL_KEYDOWN:
                switch(ev->key.keysym.scancode) {
                    case SDL_SCANCODE_UP:    input->up = TRUE; break;
                    case SDL_SCANCODE_LEFT:  input->left = TRUE; break;
                    case SDL_SCANCODE_DOWN:  input->down = TRUE; break;
                    case SDL_SCANCODE_RIGHT: input->right = TRUE; break;
                    case SDL_SCANCODE_KP_ENTER: input->enter = TRUE; break;
                    case SDL_SCANCODE_W:     input->w = TRUE; break;
                    case SDL_SCANCODE_A:     input->a = TRUE; break;
                    case SDL_SCANCODE_S:     input->s = TRUE; break;
                    case SDL_SCANCODE_D:     input->d = TRUE; break;
                    case SDL_SCANCODE_SPACE: input->space = TRUE; break;
                    case SDL_SCANCODE_LSHIFT: input->l_shift = TRUE; break;
                    case SDL_SCANCODE_RCTRL: input->r_ctrl = TRUE; break;
                    default: break;
                }
                break;

            case SDL_KEYUP:
                switch(ev->key.keysym.scancode) {
                    case SDL_SCANCODE_UP:    input->up = FALSE; break;
                    case SDL_SCANCODE_LEFT:  input->left = FALSE; break;
                    case SDL_SCANCODE_DOWN:  input->down = FALSE; break;
                    case SDL_SCANCODE_RIGHT: input->right = FALSE; break;
                    case SDL_SCANCODE_KP_ENTER: input->enter = FALSE; break;
                    case SDL_SCANCODE_W:     input->w = FALSE; break;
                    case SDL_SCANCODE_A:     input->a = FALSE; break;
                    case SDL_SCANCODE_S:     input->s = FALSE; break;
                    case SDL_SCANCODE_D:     input->d = FALSE; break;
                    case SDL_SCANCODE_SPACE: input->space = FALSE; break;
                    case SDL_SCANCODE_LSHIFT: input->l_shift = FALSE; ; break;
                    case SDL_SCANCODE_RCTRL: input->r_ctrl = FALSE; break;
                    default: break;
                }
                break;

            default:
                break;
        }
    }

    return done;
}
