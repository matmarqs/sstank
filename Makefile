# Adapted from <https://makefiletutorial.com/#makefile-cookbook>

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -static $(shell pkg-config --cflags sdl2 SDL2_image SDL2_gfx SDL2_net)
LDFLAGS = $(shell pkg-config --libs sdl2 SDL2_image SDL2_gfx SDL2_net) -lm

SRC_DIRS := ./src
BUILD_DIR := ./build

CLIENT_TARGET := client.elf
SERVER_TARGET := server.elf

# Find all the C and C++ files we want to compile
# Note the single quotes around the * expressions. The shell will incorrectly expand these otherwise, but we want to send the * directly to the find command.
SRCS := $(shell find $(SRC_DIRS) -name '*.c')
SRCS_SHARED := $(shell find $(SRC_DIRS)/shared -name '*.c')
SRCS_CLIENT := $(shell find $(SRC_DIRS)/client -name '*.c')
SRCS_SERVER := $(shell find $(SRC_DIRS)/server -name '*.c')

# Prepends BUILD_DIR and replaces ".c" to ".o" to every src file
# As an example, ./your_dir/hello.c turns into ./build/your_dir/hello.o
OBJS := $(SRCS:./%.c=$(BUILD_DIR)/%.o)
OBJS_SHARED := $(SRCS_SHARED:./%.c=$(BUILD_DIR)/%.o)
OBJS_CLIENT := $(SRCS_CLIENT:./%.c=$(BUILD_DIR)/%.o)
OBJS_SERVER := $(SRCS_SERVER:./%.c=$(BUILD_DIR)/%.o)

# String substitution (suffix version without %).
# As an example, ./build/hello.o turns into ./build/hello.d
DEPS := $(OBJS:.o=.d)

# The -MMD and -MP flags together generate Makefiles for us!
# These files will have .d instead of .o as the output.
CPPFLAGS := -MMD -MP

all: $(CLIENT_TARGET) $(SERVER_TARGET)

# Client linking step.
$(CLIENT_TARGET): $(OBJS_CLIENT) $(OBJS_SHARED)
	$(CC) $^ -o $@ $(LDFLAGS)

# Server linking step.
$(SERVER_TARGET): $(OBJS_SERVER) $(OBJS_SHARED)
	$(CC) $^ -o $@ $(LDFLAGS)

# Build step for C source
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(CLIENT_TARGET) $(SERVER_TARGET)

.PHONY: all clean

# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(DEPS)
