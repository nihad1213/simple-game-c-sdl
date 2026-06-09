#include "asset_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/**
 * @brief Add background image.
 * 
 * This function will be add background image. This function is used to add .gif as a background
 * Falls back to single-frame loading for non-Gif formats.
 *
 * @param renderer A pointer to the SDL_Renderer struct to initialize
 * @param background A pointer to the Background struct to initialize.
 * @return true if initialization is successful, false otherwise.
 */
bool add_background_image(SDL_Renderer* renderer, Background* background) {
    int w, h, channels;

    unsigned char* data = stbi_load(background->path, &w, &h, &channels, 4);
    if (!data) {
        SDL_Log("Failed to load image: %s", background->path);
        return false;
    }

    SDL_Surface* surface = SDL_CreateSurfaceFrom(
        w,
        h,
        SDL_PIXELFORMAT_RGBA32,
        data,
        w * 4
    );

    if (!surface) {
        stbi_image_free(data);
        return false;
    }

    background->frames = SDL_CreateTextureFromSurface(renderer, surface);

    background->width = w;
    background->height = h;

    SDL_DestroySurface(surface);
    stbi_image_free(data);

    return background->frames != NULL;
}

/**
 * @brief Advance the animation based on elapsed time.
 *
 * @param background A pointer to the Background struct to update.
 */
void update_background(Background* background) {

} 

/**
 * @brief Render the current frame of the background, stretched to the window.
 *
 * @param renderer A pointer to the SDL_Renderer to draw with.
 * @param background A pointer to the Background struct to render.
 */
void render_background(SDL_Renderer* renderer, Background* background) {

}

/**
 * @brief Destroy all frame textures and free the Background's allocations.
 *
 * @param background A pointer to the Background struct to free.
 */
void free_background(Background* background) {

}