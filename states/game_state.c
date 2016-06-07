// Copyright © 2016 Victor Hermann "vitorhn" Chiletto
// Licensed under the MIT/Expat license

#include <stdbool.h>

#include "../sprite.h"
#include "../vecmath.h"
#include "game_state.h"

#include "helpers/assetloader.h"

static struct {
    bool up, down, left, right, click;
    vec2 mousepos;
} iptstate;

static struct entity player;

struct projectile {
    vec2 pos;
    vec2 mov;
    bool active;
};

static struct background background;

static struct projectile projectiles[100];

void game_init(SDL_Renderer *renderer) {
    SDL_RenderSetLogicalSize(renderer, 1280, 720);
    iptstate.up = false;
    iptstate.down = false;
    iptstate.left = false;
    iptstate.right = false;
    player.pos.x = 300;
    player.pos.y = 300;
    player.mov.x = 0;
    player.mov.y = 0;
    player.current_sprite = &player.idle;
    entity_load(renderer, "papaco", &player);

    background_load(renderer, "template", &background);
    /*
    sprite_load(&player.spr, renderer, "iddle.png");

    sprite_load(&background.spr, renderer, "template.png");

    background_load("template.txt", &background);

    entity_load("idle.txt", &player);
    */
    memset(projectiles, 0, sizeof(projectiles));
}

void game_handle(SDL_Event *event) {
    switch (event->type) {
        case SDL_KEYDOWN:
            switch (event->key.keysym.sym) {
                case SDLK_s:
                    iptstate.down = true;
                    break;
                case SDLK_w:
                    iptstate.up = true;
                    break;
                case SDLK_d:
                    iptstate.right = true;
                    break;
                case SDLK_a:
                    iptstate.left = true;
                    break;
                default:
                    break;
            }
            break;
        case SDL_KEYUP:
            switch (event->key.keysym.sym) {
                case SDLK_s:
                    iptstate.down = false;
                    break;
                case SDLK_w:
                    iptstate.up = false;
                    break;
                case SDLK_d:
                    iptstate.right = false;
                    break;
                case SDLK_a:
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

static void projectile_add(struct projectile proj) {
    for (size_t i = 0; i < 100; i++) {
        if (!projectiles[i].active) {
            projectiles[i] = proj;
            return;
        }
    }
}

static void projectiles_update(void) {
    // TODO: collide projectiles with the bg borders and entities, and set active to false
    // otherwise we're eventually going to run out of projectiles
    for (size_t i = 0; i < 100; i++) {
        if (projectiles[i].active) {
            projectiles[i].pos = sum(projectiles[i].pos, projectiles[i].mov);

            rect projcol = {
                .x = projectiles[i].pos.x,
                .y = projectiles[i].pos.y,
                .w = player.idle.w,
                .h = player.idle.h
            };

            if (projcol.y <= background.col.y ||
                projcol.y + projcol.h >= background.col.y + background.col.h ||
                projcol.x <= background.col.x ||
                projcol.x + projcol.w >= background.col.x + background.col.w)
            {
                projectiles[i].active = false;
            }
        }
    }
}

static void player_think(void) {
    vec2 newmov = {0, 0};
    bool moving = false;
    if (iptstate.up) {
        newmov.y -= 1;
        moving = true;
    } 
    if (iptstate.down) {
        newmov.y += 1;
        moving = true;
    }

    if (iptstate.right) {
        newmov.x += 1;
        moving = true;
    }
    if (iptstate.left) {
        newmov.x -= 1;
        moving = true;
    }
    newmov = unit(newmov);
    player.mov = newmov;

    player.pos = sum(player.pos, player.mov);

    if (moving) {
        player.current_sprite = &player.revolver;
    }
    else {
        player.current_sprite= &player.idle;
    }

    rect playercol = {player.pos.x, player.pos.y, player.idle.w, player.idle.h};

    if (playercol.y <= background.col.y ||
        playercol.y + playercol.h >= background.col.y + background.col.h || 
        playercol.x <= background.col.x ||
        playercol.x + playercol.w >= background.col.x + background.col.w)
    {
        vec2 unmov = player.mov;
        unmov.x = -unmov.x;
        unmov.y = -unmov.y;

        player.pos = sum(player.pos, unmov);
    }


    if (iptstate.click) {
        vec2 mov = get_vec(player.pos, iptstate.mousepos);
        struct projectile newp = {
            .pos = player.pos,
            .mov = mov,
            .active = true
        };

        projectile_add(newp);

        iptstate.click = false;
    }

    player.lookat = (pointangle(sum(player.pos, player.current_sprite->rotcenter), iptstate.mousepos) * (180/acos(-1))) - 90;
}

void game_think(void) {
    player_think();
    projectiles_update();
}

static void projectiles_paint(SDL_Renderer *renderer, unsigned diff) {
    for (size_t i = 0; i < 100; i++) {
        // TODO: correct the projectile pos according to the current diff
        if (projectiles[i].active) {
            vec2 corrected = mul(projectiles[i].mov, diff);
            corrected = sum(corrected, projectiles[i].pos);
            sprite_paint(player.current_sprite, renderer, corrected);
        }
    }
}

void game_paint(SDL_Renderer *renderer, unsigned diff) {
    vec2 corrected = mul(player.mov, diff);
    corrected = sum(corrected, player.pos);


    vec2 bpos = {0, 0};
    sprite_paint(&background.spr, renderer, bpos);
    sprite_paint_ex(player.current_sprite, renderer, corrected, player.lookat, player.current_sprite->rotcenter);
    projectiles_paint(renderer, diff);
}

void game_quit(void) {
    sprite_free(&player.idle);
}
// vim: set ts=4 sw=4 expandtab:
