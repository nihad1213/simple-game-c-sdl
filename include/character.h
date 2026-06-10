#pragma once

#include <SDL3/SDL.h>
#include <stdbool.h>

#define FRAME_WIDTH 200
#define FRAME_HEIGHT 200
#define IDLE_FRAMES 8

typedef enum {
    ANIM_IDLE,
    ANIM_RUN,
    ANIM_JUMP,
    ANIM_FALL,
    ANIM_DEATH,
    ANIM_ATTACK1,
    ANIM_ATTACK2,
    ANIM_TAKE_HIT
} AnimationStates;





typedef struct {

} Player;