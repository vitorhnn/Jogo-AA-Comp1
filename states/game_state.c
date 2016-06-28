// Copyright © 2016 Victor Hermann "vitorhn" Chiletto
// Licensed under the MIT/Expat license

#include <stdbool.h>

#include "../entity.h"
#include "../anim.h"
#include "../vecmath.h"
#include "game_state.h"

#include "helpers/assetloader.h"

static struct {
    bool up, down, left, right, click;
    vec2 mousepos, prevmouse;
} iptstate;

static entity player;

struct projectile {
    vec2 pos;
    vec2 mov;
    float angle;
    bool active;
};

static struct background background;

static struct projectile projectiles[100];

static sprite bullet;

static void player_think(entity *this);

void game_init(SDL_Renderer *renderer)
{
    SDL_RenderSetLogicalSize(renderer, 1280, 720);
    iptstate.up = false;
    iptstate.down = false;
    iptstate.left = false;
    iptstate.right = false;
    player.pos.x = 300;
    player.pos.y = 300;
    player.mov.x = 0;
    player.mov.y = 0;
    player.think = &player_think;

    entity_load(&player, renderer, "assets/characters/main");

    entity_play_anim(&player, "idle");

    background_load(renderer, "background_01", &background);

    sprite_load(&bullet, renderer, "revolver_bullet.png");

    memset(projectiles, 0, sizeof(projectiles));
}

void game_handle(SDL_Event *event)
{
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

static void projectile_add(struct projectile proj)
{
    for (size_t i = 0; i < 100; i++) {
        if (!projectiles[i].active) {
            projectiles[i] = proj;
            return;
        }
    }
}

static void projectiles_update(void)
{
    for (size_t i = 0; i < 100; i++) {
        if (projectiles[i].active) {
            projectiles[i].pos = sum(projectiles[i].pos, projectiles[i].mov);

            rect projcol = {
                .x = projectiles[i].pos.x,
                .y = projectiles[i].pos.y,
                .w = bullet.w,
                .h = bullet.h
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

static void player_think(entity *this)
{
    anim_think(this->current_anim);
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
        entity_play_anim(&player, "revolver");
    } else {
        entity_play_anim(&player, "idle");
    }

    rect playercol = {player.pos.x, player.pos.y, player.current_anim->frames[player.current_anim->curframe].w, player.current_anim->frames[player.current_anim->curframe].h};

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

    float dist = pointdistance(iptstate.mousepos, sum(player.pos, player.current_anim->spr.rotcenter));

    if (dist < 150) {
        vec2 mmov = get_vec(iptstate.prevmouse, iptstate.mousepos);
        mmov = mul(mmov, -5);

        iptstate.mousepos = sum(iptstate.mousepos, mmov);

        SDL_WarpMouseInWindow(NULL, (int) iptstate.mousepos.x, (int) iptstate.mousepos.y);
    }

    iptstate.prevmouse = iptstate.mousepos;

    // HIC SUNT DRACONES
    // really ugly math to get the projectile origin after the sprite has been rotated by SDL

    // EM NOME DE JESUS

    vec2 absrot = sum(player.pos, player.current_anim->spr.rotcenter);
    vec2 absorigin = sum(player.pos, player.current_anim->spr.projorigin);
    float distance = pointdistance(absorigin, absrot);
    float angle = pointangle(absrot, absorigin) + player.lookat;
    vec2 rotorigin = {
        // this should really be summing (https://math.stackexchange.com/questions/475917/how-to-find-position-of-a-point-based-on-known-angle-radius-and-center-of-rotat)
        // but after 4 hours trying to get it right, and after wrongly typing - and getting the right number
        // I'm just going to accept that using - here is correct and move on
        .x = absrot.x - distance * cosf(angle),
        .y = absrot.y - distance * sinf(angle)
    };

    if (iptstate.click) {
        entity_play_anim(this, "revolver_shot");
        iptstate.click = false;
    }

    if (player.current_anim->projspawned) {
        vec2 mov = get_vec(rotorigin, iptstate.mousepos);

        struct projectile newp = {
            .pos = rotorigin,
            .mov = mul(mov, 3.5),
            .angle = pointangle(rotorigin, iptstate.mousepos) - (acosf(-1) / 2),
            .active = true
        };

        projectile_add(newp);

        player.current_anim->projspawned = false;
    }

    player.lookat = pointangle(rotorigin, iptstate.mousepos) - (acosf(-1) / 2);
}

void game_think(void)
{
    player.think(&player);
    projectiles_update();
}

static void projectiles_paint(SDL_Renderer *renderer, unsigned diff)
{
    for (size_t i = 0; i < 100; i++) {
        // TODO: correct the projectile pos according to the current diff
        if (projectiles[i].active) {
            vec2 corrected = mul(projectiles[i].mov, diff);
            corrected = sum(corrected, projectiles[i].pos);
            sprite_paint_less_ex(&bullet, renderer, corrected, projectiles[i].angle);
        }
    }
}

void game_paint(SDL_Renderer *renderer, unsigned diff)
{
    vec2 corrected = mul(player.mov, diff);
    corrected = sum(corrected, player.pos);


    vec2 bpos = {0, 0};
    sprite_paint(&background.spr, renderer, bpos);
    anim_paint(player.current_anim, renderer, player.pos, player.lookat);

    projectiles_paint(renderer, diff);
}

void game_quit(void)
{
    entity_free(&player);
}

// vim: set ts=4 sw=4 expandtab:
