#include "asset_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/**
 * @brief Add background image.
 * 
 * This function will be add background image. This function is used to add .gif as a background
 *
 * @param renderer A pointer to the SDL_Renderer struct to initialize
 * @param background A pointer to the Background struct to initialize.
 * @return true if initialization is successful, false otherwise.
 */
bool add_background_image(SDL_Renderer* renderer, Background* background) {
    SDL_Surface * IMG_LoadGIF_IO(SDL_IOStream *src);
    
    return true;
}