#pragma once

#include <SDL3/SDL.h>
#include <stdbool.h>

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    const char* title;
    int width;
    int height;
    bool is_running;
} Engine;

bool engine_init(Engine* engine, const char* title, int width, int height);
void engine_cleanup(Engine* engine);