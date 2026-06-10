#include "asset_loader.h"

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
    size_t len = SDL_strlen(background->path);
    bool is_gif = len >= 4 &&
                  SDL_strcasecmp(background->path + len - 4, ".gif") == 0;

    if (is_gif) {
        IMG_Animation* anim = IMG_LoadAnimation(background->path);
        if (!anim) {
            SDL_Log("Failed to load GIF: %s", background->path);
            return false;
        }

        background->frames = SDL_malloc(sizeof(SDL_Texture*) * anim->count);
        background->delays = SDL_malloc(sizeof(int) * anim->count);

        if (!background->frames || !background->delays) {
            SDL_free(background->frames);
            SDL_free(background->delays);
            IMG_FreeAnimation(anim);
            return false;
        }

        background->width = anim->w;
        background->height = anim->h;
        background->frame_count = anim->count;
        background->current_frame = 0;
        background->last_frame_time = SDL_GetTicks();

        for (int i = 0; i < anim->count; i++) {
            background->delays[i] = anim->delays[i];
            background->frames[i] = SDL_CreateTextureFromSurface(renderer, anim->frames[i]);
            if (!background->frames[i]) {
                for (int j = 0; j < i; j++) SDL_DestroyTexture(background->frames[j]);
                SDL_free(background->frames);
                SDL_free(background->delays);
                IMG_FreeAnimation(anim);
                return false;
            }
        }

        IMG_FreeAnimation(anim);
        return true;
    }

    SDL_Surface* surface = IMG_Load(background->path);
    if (!surface) {
        SDL_Log("Failed to load image: %s", background->path);
        return false;
    }

    background->frames = SDL_malloc(sizeof(SDL_Texture*));
    background->delays = NULL;

    if (!background->frames) {
        SDL_DestroySurface(surface);
        return false;
    }

    background->frames[0] = SDL_CreateTextureFromSurface(renderer, surface);
    background->frame_count = 1;
    background->current_frame = 0;
    background->last_frame_time = 0;
    background->width = surface->w;
    background->height = surface->h;

    SDL_DestroySurface(surface);
    return background->frames[0] != NULL;
}

/**
 * @brief Destroy all frame textures and free the Background's allocations.
 *
 * @param background A pointer to the Background struct to free.
 */
void free_background(Background* background) {
    for (int i = 0; i < background->frame_count; i++) {
        if (background->frames[i]) SDL_DestroyTexture(background->frames[i]);
    }

    SDL_free(background->frames);
    SDL_free(background->delays);

    background->frames = NULL;
    background->delays = NULL;
    background->frame_count = 0;
}