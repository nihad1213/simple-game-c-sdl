#include <engine.h>
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
