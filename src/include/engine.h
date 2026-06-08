#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <stdbool.h>

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    char* title;
    int width;
    int height;
    bool is_running;
} Engine;

bool engine_init(Engine* engine, const char* title, int width, int height);
bool engine_window_init();
void engine_cleanup();