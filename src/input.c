#include "input.h"
#include <SDL2/SDL_events.h>

void Input_InitKeys(Input *input) {
    input->up = input->left = input->down = input->right = input->enter = FALSE;
    input->w = input->a = input->s = input->d = input->space = FALSE;
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
                    case KEY_UP:    input->up = TRUE; break;
                    case KEY_LEFT:  input->left = TRUE; break;
                    case KEY_DOWN:  input->down = TRUE; break;
                    case KEY_RIGHT: input->right = TRUE; break;
                    case KEY_ENTER: input->enter = TRUE; break;
                    case KEY_W:     input->w = TRUE; break;
                    case KEY_A:     input->a = TRUE; break;
                    case KEY_S:     input->s = TRUE; break;
                    case KEY_D:     input->d = TRUE; break;
                    case KEY_SPACE: input->space = TRUE; break;
                    default: break;
                }
                break;

            case SDL_KEYUP:
                switch(ev->key.keysym.scancode) {
                    case KEY_UP:    input->up = FALSE; break;
                    case KEY_LEFT:  input->left = FALSE; break;
                    case KEY_DOWN:  input->down = FALSE; break;
                    case KEY_RIGHT: input->right = FALSE; break;
                    case KEY_ENTER: input->enter = FALSE; break;
                    case KEY_W:     input->w = FALSE; break;
                    case KEY_A:     input->a = FALSE; break;
                    case KEY_S:     input->s = FALSE; break;
                    case KEY_D:     input->d = FALSE; break;
                    case KEY_SPACE: input->space = FALSE; break;
                    default: break;
                }
                break;

            default:
                break;
        }
    }

    return done;
}
