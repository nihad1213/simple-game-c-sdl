#pragma once

#include <SDL3/SDL.h>
#include <stdbool.h>

#define FRAME_WIDTH 200
#define FRAME_HEIGHT 200
#define IDLE_FRAMES 8

/**
 * @brief AnimationStates defines available animations for a character
 */
typedef enum {
    ANIM_IDLE,       /**< Character is standing still or idle. */
    ANIM_RUN,        /**< Character is moving/running. */
    ANIM_JUMP,       /**< Character is jumping upward. */
    ANIM_FALL,       /**< Character is falling downward (in air). */
    ANIM_DEATH,      /**< Character has died or is in death animation. */
    ANIM_ATTACK1,    /**< Character performs the first attack action. */
    ANIM_ATTACK2,    /**< Character performs the second attack action. */
    ANIM_TAKE_HIT,   /**< Character is reacting to damage. */
    ANIM_COUNT       /**< Total number of animation states. */
} AnimationStates;

/**
 * @brief Represents a sprite-based animation system for textures.
 */
typedef struct {
    SDL_Texture* sheet;      /**< Single sheet or spritesheet used for animation. */
    int frame_count;           /**< Total number of frames in the animation. */
    int frame_width;           /**< Width of a single animation frame in pixels. */
    int frame_height;          /**< Height of a single animation frame in pixels. */
    int current_frame;         /**< Index of the currently active frame. */
    Uint32 last_frame_time;    /**< Timestamp of the last frame update (in milliseconds). */
    int frame_delay_ms;        /**< Time delay between frames in milliseconds. */
} Animation;

/**
 * @brief Represents a playable character in the game world.
 */
typedef struct {
    float x, y;                         /**< Position of the player in world coordinates. */
    float vel_x, vel_y;                 /**< Velocity of the player on X and Y axes. */
    float scale;                        /**< Scale of character */
    bool facing_right;                  /**< Direction the player is facing. */
    bool on_ground;                     /**< Whether the player is touching the ground. */
    Animation animations[ANIM_COUNT];   /**< Array of animations indexed by AnimationStates enum. */
    AnimationStates state;              /**< Current animation state of the player. */
    SDL_FRect hitbox;                   /**< Tight bounding box around visible character (world space). */
    SDL_FRect attack_box;               /**< Active hitbox extended forward during attack frames. */
    int  health;                        /**< Current health points. */
    bool is_attacking;                  /**< True while attack animation is playing. */
    int  which_attack;                  /**< 1 or 2, which attack was triggered. */
    bool attack_landed;                 /**< Prevents the same swing from hitting twice. */
    bool is_taking_hit;                 /**< True while hit-stun animation is playing. */
    Uint64 hit_timer;                   /**< Timestamp when hit-stun started. */
} Player;


/**
 * @brief Path, frame count, and frame delay for a single animation.
 */
typedef struct {
    const char* path;       /**< Path to the sprite sheet. */
    int frame_count;        /**< Number of frames in the sheet. */
    int frame_delay_ms;     /**< Milliseconds per frame. */
} PlayerAnimDef;


bool  player_init(Player* player, SDL_Renderer* renderer, float x, float y, bool facing_right, const PlayerAnimDef defs[ANIM_COUNT]);
void  player_handle_input(Player* player, const bool* keys, SDL_Scancode left, SDL_Scancode right,
     SDL_Scancode jump, SDL_Scancode attack1, SDL_Scancode attack2);
void  player_update(Player* player, int screen_w);
void  player_render(Player* player, SDL_Renderer* renderer);
void  player_free(Player* player);
bool  players_hitbox_overlap(Player* a, Player* b);
void  player_apply_hit(Player* victim, int damage);