// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#ifndef CREDITS_STATE_H
#define CREDITS_STATE_H

#include <SDL2/SDL.h>
#include "../common.h"

void credits_init(SDL_Renderer* renderer);

void credits_handle(SDL_Event* event);

void credits_think(void);

void credits_paint(SDL_Renderer* renderer, unsigned diff);

void credits_quit(void);

#endif

// vim: set ts=4 sw=4 expandtab:
