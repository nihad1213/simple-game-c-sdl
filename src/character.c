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
    player->health = 100;
    player->is_attacking = false;
    player->which_attack = 0;
    player->attack_landed = false;
    player->is_taking_hit = false;
    player->hit_timer = 0;
    player->attack_box = (SDL_FRect){0, 0, 0, 0};
    player->is_dead = false;

    for (int i = 0; i < ANIM_COUNT; i++) {
        if (!load_animation(renderer, &player->animations[i],
                            defs[i].path, defs[i].frame_count, defs[i].frame_delay_ms)) {
            return false;
        }
    }

    return true;
}


#define HB_X_FRAC 0.2f
#define HB_Y_FRAC 0.4f
#define HB_Y_START 0.40f

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

    if (player->is_dead) {
        player->state = ANIM_DEATH;
        Animation* da = &player->animations[ANIM_DEATH];
        Uint64 now = SDL_GetTicks();
        if (da->current_frame < da->frame_count - 1 &&
            now - da->last_frame_time >= (Uint64)da->frame_delay_ms) {
            da->current_frame++;
            da->last_frame_time = now;
        }
        return;
    }

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

    Uint64 now = SDL_GetTicks();

    if (player->is_taking_hit) {
        player->state = ANIM_TAKE_HIT;
        if (now - player->hit_timer >= 400)
            player->is_taking_hit = false;
    } else if (player->is_attacking) {
        player->state = (player->which_attack == 1) ? ANIM_ATTACK1 : ANIM_ATTACK2;
    } else if (!player->on_ground) {
        player->state = player->vel_y < 0 ? ANIM_JUMP : ANIM_FALL;
    } else if (player->vel_x != 0) {
        player->state = ANIM_RUN;
    } else {
        player->state = ANIM_IDLE;
    }

    Animation* anim = &player->animations[player->state];
    if (now - anim->last_frame_time >= (Uint64)anim->frame_delay_ms) {
        anim->current_frame = (anim->current_frame + 1) % anim->frame_count;
        anim->last_frame_time = now;
        if (player->is_attacking && anim->current_frame == 0)
            player->is_attacking = false;
    }

    player->attack_box = (SDL_FRect){0, 0, 0, 0};
    if (player->is_attacking) {
        AnimationStates atk = (player->which_attack == 1) ? ANIM_ATTACK1 : ANIM_ATTACK2;
        Animation* atk_anim = &player->animations[atk];
        int f = atk_anim->current_frame;
        if (f >= 1 && f <= atk_anim->frame_count - 2) {
            float box_w = player->hitbox.w * 0.8f;
            float box_h = player->hitbox.h * 0.5f;
            float box_y = player->hitbox.y + player->hitbox.h * 0.1f;
            float box_x = player->facing_right
                ? player->hitbox.x + player->hitbox.w
                : player->hitbox.x - box_w;
            player->attack_box = (SDL_FRect){box_x, box_y, box_w, box_h};
        }
    }
}

void player_apply_hit(Player* victim, int damage) {
    if (victim->is_taking_hit || victim->is_dead) return;
    victim->health -= damage;
    if (victim->health <= 0) {
        victim->health = 0;
        victim->is_dead = true;
        victim->is_attacking = false;
        victim->is_taking_hit = false;
        victim->animations[ANIM_DEATH].current_frame = 0;
        victim->animations[ANIM_DEATH].last_frame_time = SDL_GetTicks();
        return;
    }
    victim->is_taking_hit = true;
    victim->hit_timer = SDL_GetTicks();
    victim->is_attacking = false;
    victim->animations[ANIM_TAKE_HIT].current_frame = 0;
    victim->animations[ANIM_TAKE_HIT].last_frame_time = SDL_GetTicks();
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
                         SDL_Scancode jump, SDL_Scancode attack1, SDL_Scancode attack2) {
    if (player->is_dead) return;

    float speed = 4.0f;
    player->vel_x = 0;

    if (keys[left])  { player->vel_x = -speed; player->facing_right = false; }
    if (keys[right]) { player->vel_x =  speed; player->facing_right = true;  }
    if (keys[jump] && player->on_ground) {
        player->vel_y = -12.0f;
        player->on_ground = false;
    }
    if (keys[attack1] && !player->is_attacking && !player->is_taking_hit) {
        player->is_attacking = true;
        player->which_attack = 1;
        player->attack_landed = false;
        player->animations[ANIM_ATTACK1].current_frame = 0;
        player->animations[ANIM_ATTACK1].last_frame_time = SDL_GetTicks();
    }
    if (keys[attack2] && !player->is_attacking && !player->is_taking_hit) {
        player->is_attacking = true;
        player->which_attack = 2;
        player->attack_landed = false;
        player->animations[ANIM_ATTACK2].current_frame = 0;
        player->animations[ANIM_ATTACK2].last_frame_time = SDL_GetTicks();
    }

}