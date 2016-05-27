// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#include "../main.h"
#include "../settings.h"
#include "../sprite.h"
#include "credits_state.h"

static sprite credits;
static SDL_Texture* overlay = NULL;
static unsigned ticks = 0;

static void credits_rerender_overlay(SDL_Renderer* renderer, unsigned diff) {
    if (overlay == NULL) {
        if (setting_boolvalue("r_accelerated")) {
            overlay = SDL_CreateTexture(renderer,
                                        SDL_PIXELFORMAT_IYUV,
                                        SDL_TEXTUREACCESS_STATIC,
                                        1280,
                                        720);
        }
        else {
            // SDL's software renderer supports IYUV, but can't alpha modulate it.
            // try a simpler format.
            overlay = SDL_CreateTexture(renderer,
                                        SDL_PIXELFORMAT_RGBX8888,
                                        SDL_TEXTUREACCESS_STATIC,
                                        1280,
                                        720);

        }

        if (overlay == NULL) {
            show_error_msgbox("credits_rerender_overlay: failed to SDL_CreateTexture", ERROR_SOURCE_SDL);
            return;
        }
        SDL_SetTextureBlendMode(overlay, SDL_BLENDMODE_BLEND);
    }

    unsigned diffedticks = ticks + diff;
    uint8_t alpha = 0;
    if (diffedticks < 240) {
        alpha = (uint8_t) (((240 - diffedticks) / (float) 240) * 255);
    }
    else if (diffedticks < 720) {
        alpha = 0;
    }
    else if (diffedticks >= 720 && diffedticks < 960) {
        unsigned relticks = diffedticks - 720;
        alpha = (uint8_t) ((relticks / (float) 240) * 255);
    }
    else {
        alpha = 255;
    }


    SDL_SetTextureColorMod(overlay, 0, 0, 0);
    SDL_SetTextureAlphaMod(overlay, alpha);
}

void credits_init(SDL_Renderer* renderer) {
    SDL_RenderSetLogicalSize(renderer, 1280, 720);

    if (!sprite_load(&credits, renderer, "credits.png")) {
        show_error_msgbox("credits_init: failed to sprite_load credits", ERROR_SOURCE_SDL);
        exit(EXIT_FAILURE);
    }
}

void credits_handle(SDL_Event* event) {
    if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.sym == SDLK_RETURN) {
            ticks = 960; // TODO: make this less hacky
        }
    }
}

void credits_think(void) {
    ticks++;
    if (ticks >= 960) {
        engine_switch_state(STATE_MENU);
    }
}

void credits_paint(SDL_Renderer* renderer, unsigned diff) {
    credits_rerender_overlay(renderer, diff);

    vec2 pos = {0, 0};
    sprite_paint(&credits, renderer, pos);

    SDL_RenderCopy(renderer, overlay, NULL, NULL);
}

void credits_quit(void) {
    sprite_free(&credits);
    SDL_DestroyTexture(overlay);
}

// vim: set ts=4 sw=4 expandtab:
