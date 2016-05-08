// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#include <SDL2/SDL_ttf.h>

#include "3rdparty/physicsfs/extras/physfsrwops.h"

#include "main.h"
#include "texloader.h"
#include "menu_state.h"
#include "ui.h"

#define MAKERECT(label) {label.x, label.y, label.w, label.h}

typedef struct {
    SDL_Texture* tex;
    int x;
    int y;
    int w;
    int h;
} label;

static TTF_Font*    font            = NULL; 
static SDL_Texture* bg              = NULL;
static label        label_play      = {NULL, 500, 100, 0, 0};
static label        label_config    = {NULL, 500, 200, 0, 0};

void menu_init(SDL_Renderer* renderer) {
    bg = tex_load_from_file(renderer, "menubg.png");
}

void menu_handle(SDL_Event* event) {
    if (event->type == SDL_KEYDOWN) {
        engine_quit();
    }
}

void menu_think(void) {
    vec2i pos = {100, 100};
    if (ui_button(UI_ID, "dá pray", pos)) {
        engine_quit();
    }
}

void menu_paint(SDL_Renderer* renderer, unsigned diff) {
#pragma unused (diff)
    SDL_RenderCopy(renderer, bg, NULL, NULL);
}

void menu_quit(void) {
    TTF_CloseFont(font);
    SDL_DestroyTexture(label_play.tex);
    SDL_DestroyTexture(label_config.tex);
}

// vim: set ts=4 sw=4 expandtab:
