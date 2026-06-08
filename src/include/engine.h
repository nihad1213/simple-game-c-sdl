#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <stdbool.h>

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    int width;
    int height;
    bool is_running;
} Engine;

bool engine_window_init();