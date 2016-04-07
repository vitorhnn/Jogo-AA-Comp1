// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed undher the MIT/Expat license.

#ifndef MENU_STATE_H
#define MENU_STATE_H

#include <SDL2/SDL.h>
#include "common.h"

void menu_init(SDL_Renderer* renderer);

void menu_handle(SDL_Event* event);

state menu_think(void);

void menu_paint(SDL_Renderer* renderer, unsigned diff);

void menu_quit(void);

#endif

// vim: set ts=4 sw=4 expandtab:
