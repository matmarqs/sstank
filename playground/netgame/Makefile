CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -ggdb $(shell pkg-config --cflags sdl2 SDL2_image SDL2_gfx SDL2_net)
LDFLAGS = $(shell pkg-config --libs sdl2 SDL2_image SDL2_gfx SDL2_net) -lm

all: client.elf server.elf

client.elf: ./src/client.c
	$(CC) $^ -o $@ $(LDFLAGS) $(CFLAGS)

server.elf: ./src/server.c
	$(CC) $^ -o $@ $(LDFLAGS) $(CFLAGS)

clean:
	rm -rf client.elf server.elf

.PHONY: all clean
