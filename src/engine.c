#include "engine.h"
#include "asset_loader.h"
#include "character.h"
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

    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
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
    #define GIF_SPEED_FACTOR 2.0f

    Player p1;
    PlayerAnimDef p1_defs[ANIM_COUNT] = {
        [ANIM_IDLE]     = { "assets/p1/Sprites/Idle.png",     8, 100 },
        [ANIM_RUN]      = { "assets/p1/Sprites/Run.png",      8,  5 },
        [ANIM_JUMP]     = { "assets/p1/Sprites/Jump.png",     4, 100 },
        [ANIM_FALL]     = { "assets/p1/Sprites/Fall.png",     4, 100 },
        [ANIM_ATTACK1]  = { "assets/p1/Sprites/Attack1.png",  6,  80 },
        [ANIM_ATTACK2]  = { "assets/p1/Sprites/Attack2.png",  6,  80 },
        [ANIM_TAKE_HIT] = { "assets/p1/Sprites/Take Hit.png", 4, 100 },
        [ANIM_DEATH]    = { "assets/p1/Sprites/Death.png",    6, 120 },
    };

    Player p2;
    PlayerAnimDef p2_defs[ANIM_COUNT] = {
        [ANIM_IDLE]     = { "assets/p2/Sprites/Idle.png",     4, 200 },
        [ANIM_RUN]      = { "assets/p2/Sprites/Run.png",      8,  5 },
        [ANIM_JUMP]     = { "assets/p2/Sprites/Jump.png",     2, 200 },
        [ANIM_FALL]     = { "assets/p2/Sprites/Fall.png",     2, 200 },
        [ANIM_ATTACK1]  = { "assets/p2/Sprites/Attack1.png",  4,  120 },
        [ANIM_ATTACK2]  = { "assets/p2/Sprites/Attack2.png",  4,  120 },
        [ANIM_TAKE_HIT] = { "assets/p2/Sprites/Take hit.png", 3, 100 },
        [ANIM_DEATH]    = { "assets/p2/Sprites/Death.png",    7, 120 },
    };

    if (!player_init(&p1, engine->renderer, 50.0f, 442.0f, true, p1_defs)) {
        SDL_Log("Failed to init player 1");
        return false;
    }

    if (!player_init(&p2, engine->renderer, 800.0f, 442.0f, false, p2_defs)) {
        SDL_Log("Failed to init player 2");
        return false;
    }

    SDL_Event event;
    Background bg = {
        .path = "assets/background.gif"
    };

    add_background_image(engine->renderer, &bg);

    while (engine->is_running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT || event.key.key == SDLK_ESCAPE) {
                engine->is_running = false;
            }
        }

        Uint64 now = SDL_GetTicks();
        if (bg.frame_count > 1 && bg.delays) {
            if (now - bg.last_frame_time >= (Uint64)bg.delays[bg.current_frame] * GIF_SPEED_FACTOR) {
                bg.current_frame = (bg.current_frame + 1) % bg.frame_count;
                bg.last_frame_time = now;
            }
        }

        SDL_RenderClear(engine->renderer);
        SDL_RenderTexture(engine->renderer, bg.frames[bg.current_frame], NULL, NULL);
        

        player_update(&p1);
        player_render(&p1, engine->renderer);
        
        player_update(&p2);
        player_render(&p2, engine->renderer);

        const bool* keys = SDL_GetKeyboardState(NULL);
        player_handle_input(&p1, keys, SDL_SCANCODE_A, SDL_SCANCODE_D, SDL_SCANCODE_W);
        player_handle_input(&p2, keys, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_UP);
        

        SDL_RenderPresent(engine->renderer);
    }

    player_free(&p1);
    player_free(&p2);
    free_background(&bg);
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
    if (engine->renderer) SDL_DestroyRenderer(engine->renderer);
    if (engine->window) SDL_DestroyWindow(engine->window);

    SDL_Quit();
}