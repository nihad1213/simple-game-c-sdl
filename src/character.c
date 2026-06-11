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

bool player_init(Player* player, SDL_Renderer* renderer, float x, float y, bool facing_right, 
    const PlayerSpritePaths* paths) {

    player->x = x;
    player->y = y;
    player->vel_x = 0;
    player->vel_y = 0;
    player->facing_right = facing_right;
    player->on_ground = true;
    player->state = ANIM_IDLE;
    player->scale = 2.0f;

    struct { const char* path; int frames; int delay; } defs[ANIM_COUNT] = {
        [ANIM_IDLE] = { paths->idle, 8, 100 },
        [ANIM_RUN] = { paths->run, 8, 80 },
        [ANIM_JUMP] = { paths->jump, 4, 100 },
        [ANIM_FALL] = { paths->fall, 4, 100 },
        [ANIM_ATTACK1] = { paths->attack1, 6, 80 },
        [ANIM_ATTACK2] = { paths->attack2, 6, 80 },
        [ANIM_TAKE_HIT] = { paths->take_hit, 4, 100 },
        [ANIM_DEATH] = { paths->death, 6, 120 },
    };

    for (int i = 0; i < ANIM_COUNT; i++) {
        if (!load_animation(renderer, &player->animations[i],
                            defs[i].path, defs[i].frames, defs[i].delay)) {
            return false;
        }
    }

    return true;

}