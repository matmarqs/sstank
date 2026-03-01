#ifndef COMMON_H
#define COMMON_H

#include <math.h>

#define CONST_PI 3.14159265

#define TRUE  (1)
#define FALSE (0)

#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

#define SUCCESS (1)
#define FAILURE (0)

#define WORLD_WIDTH  (2880)
#define WORLD_HEIGHT (1800)

#define GAME_TITLE  "Projectile Game"

typedef struct Game Game;

#endif
