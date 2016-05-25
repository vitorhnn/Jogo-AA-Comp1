// Copyright © 2016 Victor Hermann "vitorhn" Chiletto
// Licensed under the MIT/Expat license

#include <stdbool.h>

#include "sprite.h"
#include "game_state.h"

static struct {
    bool up, down, left, right, click;
    vec2i mousepos;
} iptstate;

static struct {
    sprite spr;
    vec2i pos;
} player;

void game_init(SDL_Renderer *renderer) {
    iptstate.up = false;
    iptstate.down = false;
    iptstate.left = false;
    iptstate.right = false;
    sprite_load(&player.spr, renderer, "player.png");
}

void game_handle(SDL_Event *event) {
    switch (event->type) {
        case SDL_KEYDOWN:
            switch (event->key.keysym.sym) {
                case SDLK_DOWN:
                    iptstate.down = true;
                    break;
                case SDLK_UP:
                    iptstate.up = true;
                    break;
                case SDLK_RIGHT:
                    iptstate.right = true;
                    break;
                case SDLK_LEFT:
                    iptstate.left = true;
                    break;
                default:
                    break;
            }
            break;
        case SDL_KEYUP:
            switch (event->key.keysym.sym) {
                case SDLK_DOWN:
                    iptstate.down = false;
                    break;
                case SDLK_UP:
                    iptstate.up = false;
                    break;
                case SDLK_RIGHT:
                    iptstate.right = false;
                    break;
                case SDLK_LEFT:
                    iptstate.left = false;
                    break;
                default:
                    break;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            switch (event->button.button) {
                case SDL_BUTTON_LEFT:
                    iptstate.click = true;
                    break;
                default:
                    break;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            switch (event->button.button) {
                case SDL_BUTTON_LEFT:
                    iptstate.click = false;
                    break;
                default:
                    break;
            }
            break;
    }
}

void game_think(void) {
    if (iptstate.up) {
        player.pos.y -= 1;
    } 
    else if (iptstate.down) {
        player.pos.y += 1;
    }

    if (iptstate.right) {
        player.pos.x += 1;
    }
    else if (iptstate.left) {
        player.pos.x -= 1;
    }
}

void game_paint(SDL_Renderer *renderer, unsigned diff) {
    vec2i corrected = player.pos;
    if (iptstate.up) {
        corrected.y -= 1 * diff;
    } 
    else if (iptstate.down) {
        corrected.y += 1 * diff;
    }

    if (iptstate.right) {
        corrected.x += 1 * diff;
    }
    else if (iptstate.left) {
        corrected.x -= 1 * diff;
    }
    sprite_paint(&player.spr, renderer, player.pos);
}

void game_quit(void) {
    sprite_free(&player.spr);
}
// vim: set ts=4 sw=4 expandtab:
