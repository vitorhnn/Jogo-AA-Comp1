// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/X11 license.

#include "texloader.h"
#include "credits_state.h"

static SDL_Texture* credits = NULL;

void credits_init(SDL_Renderer* renderer) {
    SDL_RenderSetLogicalSize(renderer, 1280, 720);
    credits = tex_load_from_file(renderer, "credits.png");
    if (credits == NULL) {
       show_error_msgbox("credits_init: failed to tex_load_from_file credits", ERROR_SOURCE_SDL);
       exit(EXIT_FAILURE);
    }
}

void credits_handle(SDL_Event* event) {
#pragma unused (event)
}

state credits_think(void) {
    return STATE_NOCHANGE;
}

void credits_paint(SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, credits, NULL, NULL);
}

void credits_quit(void) {
    SDL_DestroyTexture(credits);
}

// vim: set ts=4 sw=4 expandtab:
