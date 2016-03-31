// Copyright © Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdbool.h>
#include <SDL2/SDL_render.h>

typedef struct {
    int width;
    int height;
    SDL_RendererFlags renderflags;
} settings_t; // _t is evil, I get it.

void settings_init(int argc, char** argv);

settings_t* settings_get_settings_ptr(void);

void settings_disk_sync(void);

void settings_quit(void);


#endif

//vim: set ts=4 sw=4 expandtab:
