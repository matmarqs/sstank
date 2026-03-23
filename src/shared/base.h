#ifndef _BASE_H
#define _BASE_H

#include <stdint.h>

#include "base_debug.h"
#include "base_config.h"

#define SERVER_PORT 5555

#define CONST_PI 3.14159265

#define TRUE 1
#define FALSE 0

#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

#define SUCCESS 1
#define FAILURE 0

#define UNUSED(x) (void)(x)

#endif
