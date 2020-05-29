// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#include <SDL2/SDL_ttf.h>

#include "../3rdparty/physicsfs/extras/physfsrwops.h"

#include "../main.h"
#include "menu_state.h"
#include "../sprite.h"
#include "../ui.h"

static sprite bg;

void menu_init(void)
{
    sprite_load(&bg, "assets/menus/menus/main_menu.png");
}

void menu_handle(SDL_Event *event)
{
#pragma unused (event)
    /*if (event->type == SDL_KEYDOWN) {
        engine_quit();
    }*/
}

void menu_think(void)
{
    /*
    rect r = {94, 54, 455, 116};
    SDL_Color c = {255, 255, 255, SDL_ALPHA_OPAQUE};

    if (ui_rect(UI_ID, r)) {
        engine_switch_state(STATE_GAME);
    }

    r.x = 94;
    r.y = 493;

    if (ui_rect(UI_ID, r)) {
        engine_quit();
    }
    */
}

void menu_paint(float diff)
{
#pragma unused (diff)
    vec2 pos = {0, 0};
    sprite_paint(&bg, pos);
}

void menu_quit(void)
{
    sprite_free(&bg);
}

// vim: set ts=4 sw=4 expandtab:
