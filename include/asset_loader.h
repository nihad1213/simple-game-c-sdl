#pragma once

#include <SDL3/SDL.h>

typedef struct {
    const char* path;
    SDL_Texture* texture;
    int width;
    int height;
} Background;

bool add_background_image(Background* background);
