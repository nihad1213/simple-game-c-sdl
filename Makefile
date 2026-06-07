CC := gcc
CFLAGS := -Wall -Wextra -Werror

TARGET := game

SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:.c=.o)

SDL_CFLAGS := $(shell pkg-config --cflags sdl3)
SDL_LIBS := $(shell pkg-config --libs sdl3)

CFLAGS += $(SDL_CFLAGS)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(SDL_LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)