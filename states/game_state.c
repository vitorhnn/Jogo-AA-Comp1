// Copyright © 2016 Victor Hermann "vitorhn" Chiletto
// Licensed under the MIT/Expat license

#include <stdbool.h>

#include "../sprite.h"
#include "../math.h"
#include "game_state.h"

static struct {
    bool up, down, left, right, click;
    vec2 mousepos;
} iptstate;

static struct {
    sprite spr;
    vec2 pos, mov;
} player;

struct projectile {
    vec2 pos;
    vec2 mov;
};

static struct {
    sprite spr;
    rect col;
} background;

static struct projectile projectiles[100];

void game_init(SDL_Renderer *renderer) {
    iptstate.up = false;
    iptstate.down = false;
    iptstate.left = false;
    iptstate.right = false;
    sprite_load(&player.spr, renderer, "iddle.png");

    background.col.x = 65;
    background.col.y = 90;
    background.col.w = 1150;
    background.col.h = 610;
    sprite_load(&background.spr, renderer, "template.png");


    memset(projectiles, 0, sizeof(projectiles));
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
        case SDL_MOUSEMOTION:
            iptstate.mousepos.x = event->motion.x;
            iptstate.mousepos.y = event->motion.y;
            break;
    }
}

void game_think(void) {
    vec2 newmov = {0, 0};
    if (iptstate.up) {
        newmov.y -= 1;
    } 
    else if (iptstate.down) {
        newmov.y += 1;
    }

    if (iptstate.right) {
        newmov.x += 1;
    }
    else if (iptstate.left) {
        newmov.x -= 1;
    }
    newmov = unit(&newmov);
    player.mov = newmov;

    if (iptstate.click) {
        vec2 mov = get_vec(&player.pos, &iptstate.mousepos);
        struct projectile newp = {
            .pos = player.pos,
            .mov = mov
        };

        projectiles[0] = newp;
    }

    projectiles[0].pos.x += projectiles[0].mov.x;
    projectiles[0].pos.y += projectiles[0].mov.y;
}

void game_paint(SDL_Renderer *renderer, unsigned diff) {
    vec2 corrected = player.pos;


    vec2 bpos = {0, 0};
    sprite_paint(&background.spr, renderer, bpos);
    sprite_paint(&player.spr, renderer, player.pos);
    sprite_paint(&player.spr, renderer, projectiles[0].pos);
}

void game_quit(void) {
    sprite_free(&player.spr);
}
// vim: set ts=4 sw=4 expandtab:
