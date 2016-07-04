// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#ifndef UI_H
#define UI_H

#include <stdbool.h>

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_events.h>
#include "vecmath.h"

#ifdef UI_NOCONFLICT
#define UI_ID (UI_NOCONFLICT + __LINE__)
#else
#define UI_ID (__LINE__)
#endif

void ui_init(void);

void ui_handle(SDL_Event *event);

void ui_think(void);

void ui_paint(SDL_Renderer *renderer);

void ui_quit(void);

bool ui_rect(int id, rect rect);

bool ui_button(int id, const char *text, vec2 pos, SDL_Color color);

#endif
