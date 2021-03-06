// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <SDL2/SDL.h>
#include "../sprite.h"

void game_init(SDL_Renderer *renderer);

void game_handle(SDL_Event *event);

void game_think(void);

void game_paint(SDL_Renderer *renderer, unsigned diff);

void game_quit(void);

#endif

// vim: set ts=4 sw=4 expandtab:
