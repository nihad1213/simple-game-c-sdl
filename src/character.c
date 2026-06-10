#include <SDL3_image/SDL_image.h>
#include "character.h"

/**
 * @brief Loads a sprite sheet and initializes an animation object.
 *
 * This function loads an image file from disk, converts it into an SDL texture,
 * and initializes animation metadata such as frame count, frame size, and timing.
 *
 * The sprite sheet is assumed to be arranged horizontally, where all frames
 * are evenly distributed across the image width.
 *
 * @param renderer The SDL renderer used to create GPU textures.
 * @param animation Pointer to the Animation struct to initialize.
 * @param path File path to the sprite image.
 * @param frame_count Number of frames in the sprite sheet.
 * @param frame_delay_ms Delay between animation frames in milliseconds.
 *
 * @return true if the animation was loaded successfully, false otherwise.
 */
static bool load_anumation(SDL_Renderer* renderer, Animation* animation, 
                            const char* path, int frame_count, int frame_delay_ms) {
    
    SDL_Surface* surface = IMG_Load(path);

    if (!surface) {
        SDL_Log("Failed to load sprite");
        return false;
    }

    animation->texture = SDL_CreateTextureFromSurface(renderer, surface);
    animation->frame_count = frame_count;
    animation->frame_width = surface->w / frame_count;
    animation->current_frame = 0;
    animation->last_frame_time = SDL_GetTicks();
    animation->frame_delay_ms = frame_delay_ms;

    SDL_Destroy(surface);

    return animation->texture != NULL;
}