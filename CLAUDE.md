# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build & Run

```bash
make          # compile to bin/game
make run      # compile and run
make clean    # remove build/ and bin/
```

Dependencies: SDL3 and SDL3_image, resolved via `pkg-config`. Both must be installed system-wide.

The game must be launched from the project root so relative asset paths (`assets/...`) resolve correctly.

## Architecture

`main.c` creates an `Engine`, calls `engine_init` → `engine_run` → `engine_cleanup`, then exits.

**Engine** (`src/engine.c`, `include/engine.h`) owns the SDL window and renderer, runs the main loop, and drives all subsystems. It creates both players and the background inside `engine_run`, then advances them each frame.

**Player / Character** (`src/character.c`, `include/character.h`) holds a `Player` struct with position, velocity, scale, facing direction, and an `Animation` array indexed by `AnimationStates` enum (`ANIM_IDLE`, `ANIM_RUN`, etc.). Each animation stores its spritesheet as a single `SDL_Texture*` and advances frames on a millisecond timer. Black pixels (`0,0,0`) are treated as transparent via `SDL_SetSurfaceColorKey`. Rendering uses `SDL_RenderTextureRotated` with `SDL_FLIP_HORIZONTAL` when `facing_right` is false.

**Asset Loader** (`src/asset_loader.c`, `include/asset_loader.h`) handles the animated GIF background via `IMG_LoadAnimation` and falls back to single-frame loading for static images. Frame delays are stored per-frame and respected in the engine loop with a configurable `GIF_SPEED_FACTOR`.

### Adding a new animation state
1. Add the enum value before `ANIM_COUNT` in `include/character.h`.
2. Add the sprite path field to `PlayerSpritePaths`.
3. Add the `{ path, frames, delay }` entry to the `defs[]` table in `player_init` (`src/character.c`).
4. Add the asset path when constructing `PlayerSpritePaths` in `engine_run` (`src/engine.c`).
