#include "asset_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/**
 * @brief Add background image.
 * 
 * This function will be add background image. This function is used to add .gif as a background
 * 
 * @param background A pointer to the Background struct to initialize.
 * @return true if initialization is successful, false otherwise.
 */
bool add_background_image(Background* background) {
    SDL_Surface * IMG_LoadGIF_IO(SDL_IOStream *src);
    
    return true;
}