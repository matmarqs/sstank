# Flexible auto-discovery Makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g $(shell pkg-config --cflags sdl2 SDL2_image SDL2_gfx SDL2_net)
LDFLAGS = $(shell pkg-config --libs sdl2 SDL2_image SDL2_gfx SDL2_net) -lm
TARGET = game.elf

# Find ALL .c files recursively in src/ (if you add subdirectories later)
SOURCES = $(shell find src -name "*.c" -not -name "game_init.c" -not -name "server.c")
OBJECTS = $(SOURCES:.c=.o)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

info:
	@echo "Building from: $(SOURCES)"

.PHONY: run info
