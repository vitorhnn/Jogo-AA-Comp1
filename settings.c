// Copyright © Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license

#define _GNU_SOURCE // getopt_long. maybe we should use a portable version?

#include <getopt.h>

#include "common.h"
#include "settings.h"

static settings_t* settings;

void settings_init(int argc, char** argv) {
    settings = xmalloc(sizeof(settings_t));

    // set some sane defaults

    settings->width       = 1280;
    settings->height      = 720;
    settings->renderflags = 0;
    struct option longopts[] = {
        {"width",           required_argument, 0,       'w'},
        {"height",          required_argument, 0,       'h'},
        {"accelerated",     no_argument,       0,       'a'},
        {"softrenderer",    no_argument,       0,       's'},
        {NULL,              0,                 NULL,     0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "w:h:as", longopts, NULL)) != -1) {
        switch (opt) {
            case 'w': {
                char* end;
                int maybe_width = (int) strtol(optarg, &end, 10);

                if (!*end) {
                    settings->width = maybe_width;
                }
                else {
                    fprintf(stderr, "settings_init: width was malformed.\nUnparseable characters: %s\n", end);
                }
                break;
            }
            case 'h': {
                char* end;
                int maybe_height = (int) strtol(optarg, &end, 10);

                if (!*end) {
                    settings->height = maybe_height;
                }
                else {
                    fprintf(stderr, "settings_init: height was malformed.\nUnparseable characters: %s\n", end);
                }
                break;
            }
            case 'a':
                settings->renderflags |= SDL_RENDERER_ACCELERATED; 
                break;
            case 's':
                settings->renderflags |= SDL_RENDERER_SOFTWARE;
            default:
                break;
        }
    }
    if (settings->renderflags == 0) {
        // assume a accelerated renderer if we haven't been told otherwise
        settings->renderflags = SDL_RENDERER_ACCELERATED;
    }
}
settings_t* settings_get_settings_ptr(void) {
    return settings;
}

void settings_disk_sync(void) {
    // TODO: actually implement :^)
}

void settings_quit(void) {
    settings_disk_sync();
    free(settings);
}

// vim: set ts=4 sw=4 expandtab:
