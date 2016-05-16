// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#include <SDL2/SDL_ttf.h>

#include "3rdparty/physicsfs/extras/physfsrwops.h"

#include "main.h"
#include "texloader.h"
#include "menu_state.h"
#include "ui.h"

static SDL_Texture* bg = NULL;

void menu_init(SDL_Renderer* renderer) {
    bg = tex_load_from_file(renderer, "menubg.png");
}

void menu_handle(SDL_Event* event) {
#pragma unused (event)
    /*if (event->type == SDL_KEYDOWN) {
        engine_quit();
    }*/
}

void menu_think(void) {
    vec2i pos = {100, 100};
    if (ui_button(UI_ID, "dá pray", pos)) {
        engine_quit();
    }
    pos.x = 300;
    pos.y = 300;
    if (ui_button(UI_ID, "asdasd", pos)) {
        SDL_ShowSimpleMessageBox(0, "joguin", "asdasd!", NULL);
    }
}

void menu_paint(SDL_Renderer* renderer, unsigned diff) {
#pragma unused (diff)
    SDL_RenderCopy(renderer, bg, NULL, NULL);
}

void menu_quit(void) {
}

// vim: set ts=4 sw=4 expandtab:
