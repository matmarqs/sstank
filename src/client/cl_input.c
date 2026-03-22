#include "cl_input.h"

void Input_InitKeys(Input *input) {
    input->up = input->left = input->down =
    input->right = input->shoot = input->change_arm = FALSE;
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
                    case SDL_SCANCODE_W:      input->up = TRUE; break;
                    case SDL_SCANCODE_A:      input->left = TRUE; break;
                    case SDL_SCANCODE_S:      input->down = TRUE; break;
                    case SDL_SCANCODE_D:      input->right = TRUE; break;
                    case SDL_SCANCODE_SPACE:  input->shoot = TRUE; break;
                    case SDL_SCANCODE_LSHIFT: input->change_arm = TRUE; break;
                    default: break;
                }
                break;

            case SDL_KEYUP:
                switch(ev->key.keysym.scancode) {
                    case SDL_SCANCODE_W:      input->up = FALSE; break;
                    case SDL_SCANCODE_A:      input->left = FALSE; break;
                    case SDL_SCANCODE_S:      input->down = FALSE; break;
                    case SDL_SCANCODE_D:      input->right = FALSE; break;
                    case SDL_SCANCODE_SPACE:  input->shoot = FALSE; break;
                    case SDL_SCANCODE_LSHIFT: input->change_arm = FALSE; break;
                    default: break;
                }
                break;

            default:
                break;
        }
    }

    return done;
}

PlayerActions Input_GetActions(Input *raw) {
    PlayerActions actions = {0};
    actions.move_left = raw->left;
    actions.move_right = raw->right;
    return actions;
}
