# Adapted from <https://makefiletutorial.com/#makefile-cookbook>

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g $(shell pkg-config --cflags sdl2 SDL2_image SDL2_gfx SDL2_net)
LDFLAGS = $(shell pkg-config --libs sdl2 SDL2_image SDL2_gfx SDL2_net) -lm

# Thanks to Job Vranish (https://spin.atomicobject.com/2016/08/26/makefile-c-projects/)
TARGET_EXEC := game.elf

BUILD_DIR := ./build
SRC_DIRS := ./src

# Find all the C and C++ files we want to compile
# Note the single quotes around the * expressions. The shell will incorrectly expand these otherwise, but we want to send the * directly to the find command.
SRCS := $(shell find $(SRC_DIRS) -name '*.c' -not -name 'server.c')

# Prepends BUILD_DIR and replaces ".c" to ".o" to every src file
# As an example, ./your_dir/hello.c turns into ./build/your_dir/hello.o
OBJS := $(SRCS:./%.c=$(BUILD_DIR)/%.o)

# String substitution (suffix version without %).
# As an example, ./build/hello.o turns into ./build/hello.d
DEPS := $(OBJS:.o=.d)

# Every folder in ./src will need to be passed to GCC so that it can find header files
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
# Add a prefix to INC_DIRS. So moduleA would become -ImoduleA. GCC understands this -I flag
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# The -MMD and -MP flags together generate Makefiles for us!
# These files will have .d instead of .o as the output.
CPPFLAGS := $(INC_FLAGS) -MMD -MP

# The final build step.
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Build step for C source
$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: clean

# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(DEPS)
