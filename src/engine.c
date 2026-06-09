#include "engine.h"
#include "asset_loader.h"
#include <stdio.h>

/**
 * @brief Initializes the game engine with the specified title and window dimensions.
 * 
 * This function uses SDL to create a window and renderer, and sets the engine's state accordingly.
 * It returns true if initialization is successful, or false if there is an error.
 * 
 * @param engine A pointer to the Engine struct to initialize.
 * @param title The title of the game window.
 * @param width The width of the game window.
 * @param height The height of the game window.
 * @return true if initialization is successful, false otherwise.
 */
bool engine_init(Engine* engine, const char* title, int width, int height) {
    SDL_Init(SDL_INIT_VIDEO);

    engine->window = SDL_CreateWindow(title, width, height, SDL_WINDOW_RESIZABLE);

    if (!engine->window) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create window: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    engine->renderer = SDL_CreateRenderer(engine->window, NULL);
    
    if (!engine->renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(engine->window);
        SDL_Quit();
        return false;
    }

    engine->title = title;
    engine->width = width;
    engine->height = height;
    engine->is_running = true;

    return true;
}

/**
 * @brief Runs the main game loop, processing events and rendering graphics.
 * 
 * This function will continue to run until the engine's is_running flag is set to false, 
 * typically in response to a quit event.
 * 
 * @param engine A pointer to the Engine struct to run.
 * @return true if the game loop runs successfully, false if there is an error.
 */
bool engine_run(Engine* engine) {
    SDL_Event event;
    Background bg = {
        .path = "/assets/background.gif"
    };

    while (engine->is_running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                engine->is_running = false;
            }
        }

        SDL_SetRenderDrawColor(engine->renderer, 0, 0, 0, 255);
        SDL_RenderClear(engine->renderer);
        SDL_RenderTexture(engine->renderer, bg.texture, NULL, NULL);
        SDL_RenderPresent(engine->renderer);
    }

    return true;
}

/**
 * @brief Cleans up the game engine, destroying the window and renderer, and quitting SDL.
 * 
 * This function should be called when the game is exiting to free resources and ensure a clean shutdown.
 * 
 * @param engine A pointer to the Engine struct to clean up.
 * @return void
 */
void engine_cleanup(Engine* engine) {
    if (engine->renderer) {
        SDL_DestroyRenderer(engine->renderer);
    }

    if (engine->window) {
        SDL_DestroyWindow(engine->window);
    }

    SDL_Quit();
}