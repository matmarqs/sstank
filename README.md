# projectile game

This is some kind of DDTank / Worms / Bad Eggs online game clone.
Written in C with SDL2. I want it to be portable to Windows and Linux at minimum.

## next step is make the game online

idea for the organization:

```
.
├── Makefile
├── README.md
├── assets/
├── common/
│   ├── common.h
│   ├── debug.c
│   ├── debug.h
│   ├── game_state.c      # Game logic (shared)
│   ├── game_state.h
│   ├── player.c
│   ├── player.h
│   ├── projectile.c
│   ├── projectile.h
│   ├── terrain.c
│   ├── terrain.h
│   └── protocol.h        # Network protocol definitions
├── client/
│   ├── client.c          # Client main + networking
│   ├── client.h
│   ├── game.c            # Client-side rendering
│   ├── game.h
│   ├── game_init.c
│   ├── input.c
│   ├── input.h
│   ├── input_mapper.h
│   ├── movement.c
│   └── movement.h
└── server/
    ├── server.c          # Server main
    └── server.h
```

Updated Makefile:
```
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -Icommon -Iclient
LDFLAGS = -lm

CLIENT_SOURCES = $(wildcard client/*.c) $(wildcard common/*.c)
CLIENT_OBJS = $(CLIENT_SOURCES:.c=.o)
CLIENT_TARGET = client.elf

SERVER_SOURCES = $(wildcard server/*.c) $(wildcard common/*.c)
SERVER_OBJS = $(SERVER_SOURCES:.c=.o)
SERVER_TARGET = server.elf

all: $(CLIENT_TARGET) $(SERVER_TARGET)

$(CLIENT_TARGET): $(CLIENT_OBJS)
	$(CC) $^ -o $@ $(LDFLAGS) $(shell pkg-config --libs sdl2 SDL2_image SDL2_gfx SDL2_net)

$(SERVER_TARGET): $(SERVER_OBJS)
	$(CC) $^ -o $@ -lSDL2_net

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(shell pkg-config --cflags sdl2 SDL2_image SDL2_gfx SDL2_net)

clean:
	rm -f $(CLIENT_OBJS) $(SERVER_OBJS) $(CLIENT_TARGET) $(SERVER_TARGET)
```
