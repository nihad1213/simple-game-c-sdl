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
        printf("Failed to create window.\n", SDL_GetError());
        SDL_Quit();
        return false;
    }

    engine->is_running = true;
    SDL_Event event;

    while (engine->is_running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                engine->is_running = false;
            }
        }
    }

    return true;
}
