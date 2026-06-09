CC := gcc

CFLAGS := -Wall -Wextra -Werror -Iinclude

TARGET := bin/game

SRC_DIR := src
BUILD_DIR := build

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

SDL_CFLAGS := $(shell pkg-config --cflags sdl3)
SDL_LIBS := $(shell pkg-config --libs sdl3)

CFLAGS += $(SDL_CFLAGS)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS) | bin
	$(CC) $(OBJS) -o $@ $(SDL_LIBS) -lm

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

bin:
	mkdir -p bin

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) bin