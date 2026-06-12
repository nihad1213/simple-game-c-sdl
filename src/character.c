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
                 const PlayerAnimDef defs[ANIM_COUNT]) {

    player->x = x;
    player->y = y;
    player->vel_x = 0;
    player->vel_y = 0;
    player->facing_right = facing_right;
    player->on_ground = true;
    player->state = ANIM_IDLE;
    player->scale = 2.0f;

    for (int i = 0; i < ANIM_COUNT; i++) {
        if (!load_animation(renderer, &player->animations[i],
                            defs[i].path, defs[i].frame_count, defs[i].frame_delay_ms)) {
            return false;
        }
    }

    return true;
}


#define HB_X_FRAC 0.5f
#define HB_Y_FRAC 0.8f
#define HB_Y_START 0.2f

static void compute_hitbox(Player* player) {
    Animation* cur = &player->animations[player->state];
    float fw = (float)cur->frame_width  * player->scale;
    float fh = (float)cur->frame_height * player->scale;
    player->hitbox.w = fw * HB_X_FRAC;
    player->hitbox.h = fh * HB_Y_FRAC;
    player->hitbox.x = player->x + (fw - player->hitbox.w) * 0.5f;
    player->hitbox.y = player->y + fh * HB_Y_START;
}

void player_update(Player* player, int screen_w) {

    if (!player->on_ground) player->vel_y += 0.5f;

    player->x += player->vel_x;
    player->y += player->vel_y;

    if (player->y >= 442.0f) {
        player->y = 442.0f;
        player->vel_y = 0;
        player->on_ground = true;
    }

    compute_hitbox(player);

    if (player->hitbox.x < 0) {
        player->x -= player->hitbox.x;
        player->hitbox.x = 0;
    }
    float right = player->hitbox.x + player->hitbox.w;
    if (right > (float)screen_w) {
        player->x -= right - (float)screen_w;
        player->hitbox.x = (float)screen_w - player->hitbox.w;
    }

    if (!player->on_ground)
        player->state = player->vel_y < 0 ? ANIM_JUMP : ANIM_FALL;
    else if (player->vel_x != 0)
        player->state = ANIM_RUN;
    else
        player->state = ANIM_IDLE;

    Animation* anim = &player->animations[player->state];
    Uint64 now = SDL_GetTicks();

    if (now - anim->last_frame_time >= (Uint64)anim->frame_delay_ms) {
        anim->current_frame   = (anim->current_frame + 1) % anim->frame_count;
        anim->last_frame_time = now;
    }
}

bool players_hitbox_overlap(Player* a, Player* b) {
    return SDL_HasRectIntersectionFloat(&a->hitbox, &b->hitbox);
}

void player_render(Player* player, SDL_Renderer* renderer) {
    Animation* anim = &player->animations[player->state];

    SDL_FRect src = {
        .x = (float)(anim->current_frame * anim->frame_width),
        .y = 0,
        .w = (float)anim->frame_width,
        .h = (float)anim->frame_height,
    };

    SDL_FRect dst = {
        .x = player->x,
        .y = player->y,
        .w = (float)anim->frame_width  * player->scale,
        .h = (float)anim->frame_height * player->scale,
    };

    SDL_RenderTextureRotated(renderer, anim->sheet, &src, &dst,
                             0.0, NULL,
                             player->facing_right ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);

#ifdef DEBUG_HITBOX
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderRect(renderer, &player->hitbox);
#endif
}

void player_free(Player* player) {
    for (int i = 0; i < ANIM_COUNT; i++) {
        if (player->animations[i].sheet) {
            SDL_DestroyTexture(player->animations[i].sheet);
            player->animations[i].sheet = NULL;
        }
    }
}

void player_handle_input(Player* player, const bool* keys, SDL_Scancode left, SDL_Scancode right,
                         SDL_Scancode jump) {
    
    float speed = 4.0f;
    player->vel_x = 0;

    if (keys[left])  { player->vel_x = -speed; player->facing_right = false; }
    if (keys[right]) { player->vel_x =  speed; player->facing_right = true;  }
    if (keys[jump] && player->on_ground) {
        player->vel_y = -12.0f;
        player->on_ground = false;
    }

}