#include "base_common.h"

#include "logic_input.h"

void Input_InitKeys(Input *input) {
    input->w = input->a = input->s = input->d = input->space = input->l_shift = FALSE;
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
                    case SDL_SCANCODE_W:     input->w = TRUE; break;
                    case SDL_SCANCODE_A:     input->a = TRUE; break;
                    case SDL_SCANCODE_S:     input->s = TRUE; break;
                    case SDL_SCANCODE_D:     input->d = TRUE; break;
                    case SDL_SCANCODE_SPACE: input->space = TRUE; break;
                    case SDL_SCANCODE_LSHIFT: input->l_shift = TRUE; break;
                    default: break;
                }
                break;

            case SDL_KEYUP:
                switch(ev->key.keysym.scancode) {
                    case SDL_SCANCODE_W:     input->w = FALSE; break;
                    case SDL_SCANCODE_A:     input->a = FALSE; break;
                    case SDL_SCANCODE_S:     input->s = FALSE; break;
                    case SDL_SCANCODE_D:     input->d = FALSE; break;
                    case SDL_SCANCODE_SPACE: input->space = FALSE; break;
                    case SDL_SCANCODE_LSHIFT: input->l_shift = FALSE; break;
                    default: break;
                }
                break;

            default:
                break;
        }
    }

    return done;
}
