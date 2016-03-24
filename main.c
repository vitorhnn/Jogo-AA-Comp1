// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/X11 license.

#include <stdbool.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <physfs.h>

#include "common.h"
#include "credits_state.h"

#define TICKS_PER_SECOND 240
#define MS_PER_UPDATE 1000 / TICKS_PER_SECOND

typedef void (*state_initializer_ptr)(SDL_Renderer*);
typedef void (*state_handler_ptr)(SDL_Event*);
typedef state (*state_thinker_ptr)(void);
typedef void (*state_painter_ptr)(SDL_Renderer*, unsigned);
typedef void (*state_quitter_ptr)(void);

typedef struct {
    state_handler_ptr handle;
    state_thinker_ptr think;
    state_painter_ptr paint;
    state_quitter_ptr quit;
} state_function_ptrs;

static bool running = true;

void engine_quit(void) {
    running = false;
}

static state_initializer_ptr engine_reevaluate_ptrs(state_function_ptrs* ptrs, state new_state) {
    state_initializer_ptr retval = NULL;
    switch (new_state) {
        case STATE_CREDITS:
            retval       = &credits_init;
            ptrs->handle = &credits_handle;
            ptrs->think  = &credits_think;
            ptrs->paint  = &credits_paint;
            ptrs->quit   = &credits_quit;
            break;
        case STATE_WTF:
        default:
            show_error_msgbox("engine_reevaluate_ptrs: called with STATE_WTF new_state", ERROR_SOURCE_INTERNAL);
            break;
    }
    return retval;
}

static int engine_run(void) {
    state current_state = STATE_CREDITS;
    state_function_ptrs ptrs;
    state_initializer_ptr init = engine_reevaluate_ptrs(&ptrs, current_state);
    
    SDL_Window*   window   = SDL_CreateWindow("joguin", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_DisableScreenSaver();

    SDL_Event event;

    init(renderer);

    unsigned then = SDL_GetTicks();
    long lag = 0;
    while (running) {
        unsigned now = SDL_GetTicks();
        unsigned diff = now - then;
        then = now;
        lag += diff;

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                default:
                    ptrs.handle(&event);
                    break;
            }
        }
        state should_switch = STATE_NOCHANGE;
        while (lag >= MS_PER_UPDATE) {
            should_switch = ptrs.think();
            lag -= MS_PER_UPDATE;
            if (should_switch != 0) {
                break;
            }
        }
        ptrs.paint(renderer, (unsigned) lag / MS_PER_UPDATE);

        SDL_RenderPresent(renderer);

        if (should_switch != 0) {
            ptrs.quit();
            state_initializer_ptr state_init = engine_reevaluate_ptrs(&ptrs, should_switch);
            state_init(renderer);
        }
    }

    return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
#pragma unused (argc)
    // start everything up
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        show_error_msgbox("failed to SDL_Init", ERROR_SOURCE_SDL);
        return EXIT_FAILURE;
    }

    if (Mix_Init(MIX_INIT_FLAC | MIX_INIT_OGG) != (MIX_INIT_FLAC | MIX_INIT_OGG) ) {
        show_error_msgbox("failed to Mix_Init", ERROR_SOURCE_SDL);
        return EXIT_FAILURE;
    }

    if (TTF_Init() != 0) {
        show_error_msgbox("failed to TTF_Init", ERROR_SOURCE_SDL);
        return EXIT_FAILURE;
    }

    if (PHYSFS_init(argv[0]) == 0) {
        show_error_msgbox("failed to PHYSFS_init", ERROR_SOURCE_PHYSFS);
        return EXIT_FAILURE;
    }

    PHYSFS_mount("data.pak", NULL, 0);
    PHYSFS_mount("loosefiles/", NULL, 0);

    int retval = engine_run();
    
    PHYSFS_deinit();
    TTF_Quit();
    Mix_Quit();
    SDL_Quit();

    return retval;
}


// vim: set ts=4 sw=4 expandtab:
