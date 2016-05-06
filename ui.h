// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#ifndef UI_H
#define UI_H

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_events.h>

#ifdef UI_NOCONFLICT
#define UI_ID (UI_NOCONFLICT + __LINE__)
#else
#define UI_ID (__LINE__)
#endif

void ui_init(void);

void ui_handle(SDL_Event *event);

void ui_paint(SDL_Renderer *renderer);

void ui_quit(void);

#endif
