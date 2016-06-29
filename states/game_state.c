// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license

#include <stdbool.h>

#include "../common.h"
#include "../stage.h"
#include "../entity.h"
#include "../anim.h"
#include "../vecmath.h"
#include "game_state.h"

static struct {
    bool up, down, left, right, click, EL;
    vec2 mousepos;
} iptstate;

static entity player;

static stage curstage;

static sprite bullet;

static void player_think(entity *this);

static SDL_Renderer *renderer_; // ugly hack

static void fake_free(entity *nothing)
{
#pragma unused (nothing)
}

static void dumb_think(entity *nothing)
{
#pragma unused (nothing)
}

static entity *make_entity(const char *path)
{
    entity *ent = xmalloc(sizeof(entity));

    memset(ent, 0, sizeof(entity));

    ent->free = &entity_free;
    ent->real_think = &dumb_think;

    entity_load(ent, renderer_, path);

    stage_add_entity(&curstage, ent);

    return ent;
}

void game_init(SDL_Renderer *renderer)
{
    renderer_ = renderer;
    SDL_RenderSetLogicalSize(renderer, 1280, 720);
    memset(&iptstate, 0, sizeof(iptstate));
    player.pos.x = 400;
    player.pos.y = 400;
    player.mov.x = 0;
    player.mov.y = 0;
    player.real_think = &player_think;
    player.free = &fake_free;

    entity_load(&player, renderer, "assets/characters/main");

    entity_play_anim(&player, "idle");

    stage_load(&curstage, renderer, "assets/background/background_01");

    stage_add_entity(&curstage, &player);

    sprite_load(&bullet, renderer, "revolver_bullet.png");


    entity *boss = make_entity("assets/characters/boss");
    entity_play_anim(boss, "idle");

    boss->pos.x = 600;
    boss->pos.y = 600;
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

                case SDLK_l:
                    iptstate.EL = true;
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

    if (iptstate.EL) {
        //background_load(renderer_, "assets/background/background_02", &background);
        iptstate.EL = false;
    }

    newmov = unit(newmov);
    this->mov = newmov;

    this->pos = sum(this->pos, this->mov);

    if (moving) {
        entity_play_anim(this, "revolver");
    } else {
        entity_play_anim(this, "idle");
    }

    if (stage_is_ent_colliding(&curstage, this)) {
        vec2 unmov = this->mov;
        unmov.x = -unmov.x;
        unmov.y = -unmov.y;

        this->pos = sum(this->pos, unmov);
    }

    if (iptstate.click) {
        entity_play_anim(this, "revolver_shot");
        iptstate.click = false;
    }

    if (this->current_anim->projspawned) {
        stage_add_projectile(&curstage, this, &bullet, iptstate.mousepos, 3.5);

        this->current_anim->projspawned = false;
    }

    this->lookat = pointangle(this->rotorigin, iptstate.mousepos) - (FPI / 2);
}

void game_think(void)
{
    stage_think(&curstage);
}

void game_paint(SDL_Renderer *renderer, unsigned diff)
{
    stage_paint(&curstage, renderer, diff);
}

void game_quit(void)
{
    stage_free(&curstage);
}

// vim: set ts=4 sw=4 expandtab:
