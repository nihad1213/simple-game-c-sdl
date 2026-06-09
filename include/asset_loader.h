#pragma once

#include <SDL3/SDL.h>

/**
 * @brief The Background struct holds an animated (or static) background image.
 */
typedef struct {
    const char* path;           /**< Path to the image file (.gif, .png, .jpg) */
    SDL_Texture** frames;       /**< Array of textures, one per GIF frame */
    int* delays;                /**< Per-frame delay in milliseconds */
    int frame_count;            /**< Number of frames (1 for static images) */
    int current_frame;          /**< Index of the frame currently displayed */
    Uint64 last_frame_time;     /**< Tick timestamp of the last frame switch */
    int width;                  /**< The width of the image in pixels */
    int height;                 /**< The height of the image in pixels */
} Background;

bool add_background_image(SDL_Renderer* renderer, Background* background);