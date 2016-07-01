// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#include <SDL2/SDL_ttf.h>

#include "../3rdparty/physicsfs/extras/physfsrwops.h"

#include "../main.h"
#include "menu_state.h"
#include "../sprite.h"
#include "../ui.h"

static sprite bg;

void menu_init(SDL_Renderer *renderer)
{
    sprite_load(&bg, renderer, "assets/menus/main_menu/main_menu.png");
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
    vec2 pos = {150, 85};
    SDL_Color c = {255, 255, 255, SDL_ALPHA_OPAQUE};

    if (ui_button(UI_ID, "GIVE PRAY", pos, c)) {
        engine_switch_state(STATE_GAME);
    }

    pos.x = 150;
    pos.y = 230;

    if (ui_button(UI_ID, "SAIR", pos, c)) {
        engine_quit();
    }
}

void menu_paint(SDL_Renderer *renderer, unsigned diff)
{
#pragma unused (diff)
    vec2 pos = {0, 0};
    sprite_paint(&bg, renderer, pos);
}

void menu_quit(void)
{
    sprite_free(&bg);
}

// vim: set ts=4 sw=4 expandtab:
