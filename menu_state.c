// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#include <SDL2/SDL_ttf.h>

#include "3rdparty/physicsfs/extras/physfsrwops.h"

#include "main.h"
#include "texloader.h"
#include "menu_state.h"

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
    SDL_RWops* ops = PHYSFSRWOPS_openRead("opensans.ttf");
    font = TTF_OpenFontRW(ops, 1, 50);

    SDL_Color color = {0, 255, 0, SDL_ALPHA_OPAQUE};
    label_play.tex      = tex_from_text(renderer, "dá pray", font, color, &label_play.w, &label_play.h);
    label_config.tex    = tex_from_text(renderer, "configura aê", font, color, &label_config.w, &label_config.h);
}

void menu_handle(SDL_Event* event) {
    if (event->type == SDL_KEYDOWN) {
        engine_quit();
    }
}

state menu_think(void) {
    return STATE_NOCHANGE;
}

void menu_paint(SDL_Renderer* renderer, unsigned diff) {
#pragma unused (diff)
    SDL_Rect r = MAKERECT(label_play);
    SDL_Rect r2 = MAKERECT(label_config);

    SDL_RenderCopy(renderer, bg, NULL, NULL);
    SDL_RenderCopy(renderer, label_play.tex, NULL, &r);
    SDL_RenderCopy(renderer, label_config.tex, NULL, &r2);
}

void menu_quit(void) {
    TTF_CloseFont(font);
    SDL_DestroyTexture(label_play.tex);
    SDL_DestroyTexture(label_config.tex);
}

// vim: set ts=4 sw=4 expandtab:
