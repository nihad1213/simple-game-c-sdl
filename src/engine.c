#include "engine.h"
#include "asset_loader.h"
#include "character.h"
#include <stdio.h>
#include <SDL3_ttf/SDL_ttf.h>

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
    TTF_Init();

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

static void render_healthbar(SDL_Renderer* renderer, int win_w, int player_num, int health) {
    float bar_w = 280.0f;
    float bar_h = 22.0f;
    float bar_y = 20.0f;
    float bar_x = (player_num == 1) ? 60.0f : (float)win_w - 60.0f - bar_w;

    float pct = health / 100.0f;
    if (pct < 0.0f) pct = 0.0f;
    float filled_w = bar_w * pct;

    SDL_FRect bg_rect = {bar_x, bar_y, bar_w, bar_h};
    SDL_SetRenderDrawColor(renderer, 60, 0, 0, 255);
    SDL_RenderFillRect(renderer, &bg_rect);

    Uint8 r = (health > 50) ? 20  : 200;
    Uint8 g = (health > 25) ? 200 : 0;
    SDL_SetRenderDrawColor(renderer, r, g, 0, 220);

    SDL_FRect fill_rect;
    if (player_num == 1) {
        fill_rect = (SDL_FRect){bar_x, bar_y, filled_w, bar_h};
    } else {
        fill_rect = (SDL_FRect){bar_x + bar_w - filled_w, bar_y, filled_w, bar_h};
    }
    SDL_RenderFillRect(renderer, &fill_rect);

    SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
    SDL_RenderRect(renderer, &bg_rect);
}

static bool is_death_done(Player* p) {
    Animation* da = &p->animations[ANIM_DEATH];
    return p->is_dead && da->current_frame == da->frame_count - 1;
}

static void render_winner(SDL_Renderer* renderer, TTF_Font* font, int win_w, int win_h, int winner) {
    const char* msg = (winner == 1) ? "PLAYER 1 WINS!"
                    : (winner == 2) ? "PLAYER 2 WINS!"
                    :"DRAW!";
    SDL_Color color = {255, 215, 0, 255};
    SDL_Surface* surf = TTF_RenderText_Blended(font, msg, 0, color);
    if (!surf) return;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    if (tex) {
        SDL_FRect dst = {
            (win_w - surf->w) / 2.0f,
            (win_h - surf->h) / 2.0f - 20.0f,
            (float)surf->w,
            (float)surf->h
        };
        SDL_RenderTexture(renderer, tex, NULL, &dst);
        SDL_DestroyTexture(tex);
    }
    SDL_DestroySurface(surf);
}

static void render_countdown(Engine* engine, Background* bg, Player* p1, Player* p2) {
#define GIF_SPEED_FACTOR 2.0f
    TTF_Font* font = TTF_OpenFont("assets/fonts/BoldPixels.ttf", 120);
    if (!font) {
        SDL_Log("Failed to load font: %s", SDL_GetError());
        return;
    }
    const char* labels[] = { "3", "2", "1", "FIGHT!" };
    int win_w, win_h;
    SDL_GetWindowSize(engine->window, &win_w, &win_h);

    SDL_Event event;
    Uint64 phase_start = SDL_GetTicks();

    for (int i = 0; i < 4 && engine->is_running; i++) {
        phase_start = SDL_GetTicks();
        Uint64 duration = (i < 3) ? 1000 : 800;

        while (SDL_GetTicks() - phase_start < duration && engine->is_running) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_EVENT_QUIT || event.key.key == SDLK_ESCAPE)
                    engine->is_running = false;
            }

            Uint64 now = SDL_GetTicks();
            if (bg->frame_count > 1 && bg->delays) {
                if (now - bg->last_frame_time >= (Uint64)bg->delays[bg->current_frame] * GIF_SPEED_FACTOR) {
                    bg->current_frame = (bg->current_frame + 1) % bg->frame_count;
                    bg->last_frame_time = now;
                }
            }

            SDL_GetWindowSize(engine->window, &win_w, &win_h);

            SDL_RenderClear(engine->renderer);
            SDL_RenderTexture(engine->renderer, bg->frames[bg->current_frame], NULL, NULL);

            player_update(p1, win_w);
            player_render(p1, engine->renderer);
            player_update(p2, win_w);
            player_render(p2, engine->renderer);

            SDL_Color white = {204, 0, 0, 0};
            SDL_Surface* surf = TTF_RenderText_Blended(font, labels[i], 0, white);
            if (surf) {
                SDL_Texture* tex = SDL_CreateTextureFromSurface(engine->renderer, surf);
                if (tex) {
                    SDL_FRect dst = {
                        (win_w - surf->w) / 2.0f,
                        (win_h - surf->h) / 2.0f,
                        (float)surf->w,
                        (float)surf->h
                    };
                    SDL_RenderTexture(engine->renderer, tex, NULL, &dst);
                    SDL_DestroyTexture(tex);
                }
                SDL_DestroySurface(surf);
            }

            SDL_RenderPresent(engine->renderer);
        }
    }

    TTF_CloseFont(font);
#undef GIF_SPEED_FACTOR
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
        [ANIM_RUN]      = { "assets/p1/Sprites/Run.png",      8,   5 },
        [ANIM_JUMP]     = { "assets/p1/Sprites/Jump.png",     2, 100 },
        [ANIM_FALL]     = { "assets/p1/Sprites/Fall.png",     2, 100 },
        [ANIM_ATTACK1]  = { "assets/p1/Sprites/Attack1.png",  6,  60 },
        [ANIM_ATTACK2]  = { "assets/p1/Sprites/Attack2.png",  6,  60 },
        [ANIM_TAKE_HIT] = { "assets/p1/Sprites/Take Hit.png", 4, 100 },
        [ANIM_DEATH]    = { "assets/p1/Sprites/Death.png",    6, 120 },
    };

    Player p2;
    PlayerAnimDef p2_defs[ANIM_COUNT] = {
        [ANIM_IDLE]     = { "assets/p2/Sprites/Idle.png",     4, 200 },
        [ANIM_RUN]      = { "assets/p2/Sprites/Run.png",      8,   5 },
        [ANIM_JUMP]     = { "assets/p2/Sprites/Jump.png",     2, 200 },
        [ANIM_FALL]     = { "assets/p2/Sprites/Fall.png",     2, 200 },
        [ANIM_ATTACK1]  = { "assets/p2/Sprites/Attack1.png",  4, 70 },
        [ANIM_ATTACK2]  = { "assets/p2/Sprites/Attack2.png",  4, 70 },
        [ANIM_TAKE_HIT] = { "assets/p2/Sprites/Take hit.png", 3, 100 },
        [ANIM_DEATH]    = { "assets/p2/Sprites/Death.png",    7, 20 },
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
    Background bg = { .path = "assets/background.gif" };
    add_background_image(engine->renderer, &bg);

    render_countdown(engine, &bg, &p1, &p2);

    TTF_Font* hud_font = TTF_OpenFont("assets/fonts/BoldPixels.ttf", 72);

    int winner = 0;
    Uint64 winner_shown_at = 0;

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

        int win_w, win_h;
        SDL_GetWindowSize(engine->window, &win_w, &win_h);

        if (!winner) {
            const bool* keys = SDL_GetKeyboardState(NULL);
            player_handle_input(&p1, keys, SDL_SCANCODE_A, SDL_SCANCODE_D, SDL_SCANCODE_W,
                                SDL_SCANCODE_F, SDL_SCANCODE_G);
            player_handle_input(&p2, keys, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_UP,
                                SDL_SCANCODE_KP_1, SDL_SCANCODE_KP_2);
        }

        SDL_RenderClear(engine->renderer);
        SDL_RenderTexture(engine->renderer, bg.frames[bg.current_frame], NULL, NULL);

        player_update(&p1, win_w);
        player_update(&p2, win_w);

        if (!winner) {
            if (p1.is_attacking && !p1.attack_landed && p1.attack_box.w > 0 &&
                SDL_HasRectIntersectionFloat(&p1.attack_box, &p2.hitbox)) {
                player_apply_hit(&p2, 10);
                p1.attack_landed = true;
            }
            if (p2.is_attacking && !p2.attack_landed && p2.attack_box.w > 0 &&
                SDL_HasRectIntersectionFloat(&p2.attack_box, &p1.hitbox)) {
                player_apply_hit(&p1, 10);
                p2.attack_landed = true;
            }

            bool p1_done = is_death_done(&p1);
            bool p2_done = is_death_done(&p2);
            if (p1_done && p2_done) { winner = 3; winner_shown_at = now; }
            else if (p1_done)       { winner = 2; winner_shown_at = now; }
            else if (p2_done)       { winner = 1; winner_shown_at = now; }
        } else if (now - winner_shown_at >= 2500) {
            engine->is_running = false;
        }

        player_render(&p1, engine->renderer);
        player_render(&p2, engine->renderer);

        render_healthbar(engine->renderer, win_w, 1, p1.health);
        render_healthbar(engine->renderer, win_w, 2, p2.health);

        if (winner && hud_font)
            render_winner(engine->renderer, hud_font, win_w, win_h, winner);

        SDL_RenderPresent(engine->renderer);
    }

    if (hud_font) TTF_CloseFont(hud_font);

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

    TTF_Quit();
    SDL_Quit();
}
