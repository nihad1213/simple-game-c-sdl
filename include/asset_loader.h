#pragma once

#include <SDL3/SDL.h>

typedef struct {
    const char* path;
} Background;

bool add_background_image(Background* background);
