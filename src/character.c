#include <SDL3_image/SDL_image.h>
#include "character.h"

static bool load_animation(SDL_Renderer* renderer, Animation* anim, const char* path, int frame_count,
                           int frame_delay_ms) {
    
    if (!path) return false;

    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        SDL_Log("Failed to load sprite!");
        return false;
    }

    SDL_SetSurfaceColorKey(surface, true, SDL_MapRGB
        (SDL_GetPixelFormatDetails(surface->format), NULL, 0, 0, 0));

    anim->sheet = SDL_CreateTextureFromSurface(renderer, surface);
    anim->frame_count = frame_count;
    anim->frame_width = surface->w / frame_count;
    anim->frame_height = surface->h;
    anim->current_frame = 0;
    anim->last_frame_time = SDL_GetTicks();
    anim->frame_delay_ms = frame_delay_ms;

    SDL_DestroySurface(surface);
    return anim->sheet != NULL;
}