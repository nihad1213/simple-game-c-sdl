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
    SDL_Texture* texture;      /**< Single texture or spritesheet used for animation. */
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
    bool facing_right;                  /**< Direction the player is facing. */
    bool on_ground;                     /**< Whether the player is touching the ground. */
    Animation animations[ANIM_COUNT];   /**< Array of animations indexed by AnimationStates enum. */
    AnimationStates state;              /**< Current animation state of the player. */
} Player;


/**
 * @brief Stores file paths for all player animation sprite sheets.
 */
typedef struct {
    const char* idle;       /**< Path to the idle animation sprite sheet. */
    const char* run;        /**< Path to the running animation sprite sheet. */
    const char* jump;       /**< Path to the jump animation sprite sheet. */
    const char* fall;       /**< Path to the falling animation sprite sheet. */
    const char* attack1;    /**< Path to the first attack animation sprite sheet. */
    const char* attack2;    /**< Path to the second attack animation sprite sheet. */
    const char* take_hit;   /**< Path to the hit reaction animation sprite sheet. */
    const char* death;      /**< Path to the death animation sprite sheet. */
} PlayerSpritePaths;


bool  player_init(Player* player, SDL_Renderer* renderer, float x, float y, bool facing_right);
void  player_update(Player* player);
void  player_render(Player* player, SDL_Renderer* renderer);
void  player_free(Player* player);