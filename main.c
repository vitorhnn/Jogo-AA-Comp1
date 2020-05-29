// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#include <stdbool.h>
#include <stdlib.h>

#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <physfs.h>

#include "main.h"
#include "ui.h"
#include "settings.h"
#include "vid.h"


#include "states/credits_state.h"
#include "states/menu_state.h"
#include "states/game_state.h"


#define MS_PER_UPDATE (unsigned) (1000 / setting_intvalue("game_tickrate"))

typedef void (*state_initializer_ptr)();
typedef void (*state_handler_ptr)(SDL_Event *);
typedef void (*state_thinker_ptr)(void);
typedef void (*state_painter_ptr)(float);
typedef void (*state_quitter_ptr)(void);

typedef struct {
    state_handler_ptr handle;
    state_thinker_ptr think;
    state_painter_ptr paint;
    state_quitter_ptr quit;
} state_function_ptrs;

static game_state current_state     = STATE_MENU;
static bool running                 = true;
static bool switch_pending          = false;

static setting r_width              = {"r_width", .value = { .intval = 1280 }, SETTING_INT};
static setting r_height             = {"r_height", .value = { .intval =  720 }, SETTING_INT};
static setting r_accelerated        = {"r_accelerated", .value = { .boolval = true }, SETTING_BOOL};
static setting r_fullscreen         = {"r_fullscreen", .value = { .boolval = false }, SETTING_BOOL};
static setting game_tickrate        = {"game_tickrate", .value = { .intval = 240 }, SETTING_INT};
static setting fps_max              = {"fps_max", .value = { .intval = 60 }, SETTING_INT};


void engine_quit(void)
{
    running = false;
}

void engine_switch_state(game_state new_state)
{
    switch_pending = true;
    current_state  = new_state;
}

static state_initializer_ptr engine_reevaluate_ptrs(state_function_ptrs *ptrs, game_state new_state)
{
    state_initializer_ptr retval = NULL;

    switch (new_state) {
        /*
        case STATE_CREDITS:
            retval          = &credits_init;
            ptrs->handle    = &credits_handle;
            ptrs->think     = &credits_think;
            ptrs->paint     = &credits_paint;
            ptrs->quit      = &credits_quit;
            break;
        */

        case STATE_MENU:
            retval          = &menu_init;
            ptrs->handle    = &menu_handle;
            ptrs->think     = &menu_think;
            ptrs->paint     = &menu_paint;
            ptrs->quit      = &menu_quit;
            break;
        /*
        case STATE_GAME:
            retval          = &game_init;
            ptrs->handle    = &game_handle;
            ptrs->think     = &game_think;
            ptrs->paint     = &game_paint;
            ptrs->quit      = &game_quit;
            break;
        */

        default:
            show_error_msgbox("engine_reevaluate_ptrs: called with STATE_WTF new_state", ERROR_SOURCE_INTERNAL);
            break;
    }

    return retval;
}

static int engine_run(void)
{
    vid_init();

    ui_init();

    state_function_ptrs ptrs;
    state_initializer_ptr init = engine_reevaluate_ptrs(&ptrs, current_state);

    SDL_Event event;

    init();

    unsigned then = SDL_GetTicks();
    long lag = 0;

    while (running) {

        if (switch_pending) {
            ptrs.quit();
            state_initializer_ptr state_init = engine_reevaluate_ptrs(&ptrs, current_state);
            state_init();

            switch_pending = false;
        }

        unsigned now = SDL_GetTicks();
        unsigned diff = now - then;
        then = now;
        lag += diff;

        vid_color draw_color = {255, 255, 255, 255};
        vid_set_draw_color(draw_color);
        vid_clear();

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;

                default:
                    ui_handle(&event);
                    ptrs.handle(&event);
                    break;
            }
        }

        while (lag >= MS_PER_UPDATE) {
            ui_think();
            ptrs.think();
            lag -= MS_PER_UPDATE;

            if (switch_pending) {
                break;
            }
        }

        ptrs.paint(lag / MS_PER_UPDATE);
        //ui_paint();

        vid_present();

        int fpsmax = setting_intvalue("fps_max");

        if (fpsmax > 0) {
            if (SDL_GetTicks() - then < 1000 / fpsmax) {
                SDL_Delay(1000 / fpsmax - (SDL_GetTicks() - then));
            }
        }
    }

    ptrs.quit();

    ui_quit();

    vid_quit();

    return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
    // start everything up

    srand(time(0));

    printf("Platform is %s\n", SDL_GetPlatform());

    SDL_version compiled;
    SDL_version linked;

    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);

    printf("Compiled with SDL %d.%d.%d\n"
           "Linking against SDL %d.%d.%d\n",
           compiled.major, compiled.minor, compiled.patch,
           linked.major, linked.minor, linked.patch);

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        show_error_msgbox("failed to SDL_Init", ERROR_SOURCE_SDL);
        return EXIT_FAILURE;
    }

    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        show_error_msgbox("failed to IMG_Init", ERROR_SOURCE_SDL);
        return EXIT_FAILURE;
    }

    if (Mix_Init(MIX_INIT_FLAC | MIX_INIT_OGG) != (MIX_INIT_FLAC | MIX_INIT_OGG) ) {
        show_error_msgbox("failed to Mix_Init", ERROR_SOURCE_SDL);
        return EXIT_FAILURE;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
        show_error_msgbox("failed to Mix_OpenAudio", ERROR_SOURCE_SDL);
        return EXIT_FAILURE;
    }
    Mix_AllocateChannels(32);
    Mix_Volume(-1, MIX_MAX_VOLUME);

    if (TTF_Init() != 0) {
        show_error_msgbox("failed to TTF_Init", ERROR_SOURCE_SDL);
        return EXIT_FAILURE;
    }

    if (PHYSFS_init(argv[0]) == 0) {
        show_error_msgbox("failed to PHYSFS_init", ERROR_SOURCE_PHYSFS);
        return EXIT_FAILURE;
    }

    PHYSFS_permitSymbolicLinks(1);

    setting_register(&r_width);
    setting_register(&r_height);
    setting_register(&r_accelerated);
    setting_register(&r_fullscreen);
    setting_register(&game_tickrate);
    setting_register(&fps_max);

    if (SDL_GetPowerInfo(NULL, NULL) == SDL_POWERSTATE_ON_BATTERY) {
        printf("main(): device is on battery, setting default framerate to 30\n");
        setting_set_float("fps_max", 30);
    }

    settings_parse_argv(argc, argv);

    PHYSFS_mount("data.pak", NULL, 0);
    PHYSFS_mount("loosefiles/", NULL, 0);

    int retval = engine_run();

    PHYSFS_deinit();
    TTF_Quit();
    Mix_Quit();
    SDL_Quit();


    settings_quit();
    return retval;
}


// vim: set ts=4 sw=4 expandtab:
