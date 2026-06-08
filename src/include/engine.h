#pragma once

#include <SDL3/SDL.h>
#include <stdbool.h>

/**
 * @brief The Engine struct holds the state of the game engine.
 */
typedef struct {
    SDL_Window* window;         /**< The main game window */
    SDL_Renderer* renderer;     /**< The renderer for drawing graphics */
    const char* title;          /**< The title of the game window */
    int width;                  /**< The width of the game window */
    int height;                 /**< The height of the game window */ 
    bool is_running;            /**< Flag to indicate if the engine is running */
} Engine;

bool engine_init(Engine* engine, const char* title, int width, int height);
void engine_cleanup(Engine* engine);