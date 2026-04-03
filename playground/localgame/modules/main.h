#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
//#include <SDL2/SDL_ttf.h>
//#include <SDL2/SDL_mixer.h>

/* Meus macros para esclarecer a vida */
#define TRUE      1
#define FALSE     0

#define WIN_WIDTH   (640)
#define WIN_HEIGHT  (480)
#define SPEED       (300)       /* speed in pixels/sec */

#define HERO_NUM_FRAMES  2
#define HERO_IDLE        0
#define HERO_DOWN        1

/* Hero structure for the Main Character (MC) */
typedef struct {
    /* Variables */
    float x, y;
    float dx, dy;
    int   w, h;
    char *name;
    int health;
    int anim;
    int facingLeft;
    int dead;

    /* Texture */
    SDL_Texture *frame[HERO_NUM_FRAMES];
} Hero;

typedef struct {
    int up, down, left, right;
} Input;

typedef struct {
    /* Variables */
    float scrollX, scrollY; /* position of the camera */
    Hero mc;

    /* Input */
    Input input;
    SDL_Event event;

    /* Images */

    /* Time */
    int time, state;

    /* Graphics */
    SDL_Window *win;
    SDL_Renderer *rendr;
} Game;
