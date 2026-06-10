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
    ANIM_TAKE_HIT    /**< Character is reacting to damage. */
} AnimationStates;

typedef struct {
    SDL_Texture* texture;
    int frame_count;
    int frame_width;
    int frame_height;
    int current_frame;
    Uint32 last_frame_time;
    int frame_delay_ms;
} Animation;
