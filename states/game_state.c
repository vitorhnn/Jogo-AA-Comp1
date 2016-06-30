// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license

#include <stdbool.h>

#include "../ui.h"
#include "../common.h"
#include "../stage.h"
#include "../entity.h"
#include "../anim.h"
#include "../vecmath.h"
#include "game_state.h"

#define PLAYER_HP 500

static struct {
    bool up, down, left, right, click, EL;
    vec2 mousepos;
} iptstate;

static entity player;

static stage curstage;

static sprite bullet;

static sprite shell;

static int btimeframes;

static bool isbtime;

static void player_think(entity *this);

static SDL_Renderer *renderer_; // ugly hack

static vec2 spawns[] = {
    {48, 358},
    {547, 96},
    {1211, 390},
    {635, 694}
};

static entity *spawnerarray[4];

static void fake_free(entity *nothing)
{
#pragma unused (nothing)
}

static void dumb_think(entity *nothing)
{
#pragma unused (nothing)
}

static void gunslinger_think(entity *this)
{
    if (!this->dead) {
        entity_play_anim(this, "shot");

        vec2 target = sum(player.pos, player.current_anim->spr.rotcenter);
        if (this->current_anim->projspawned) {
            stage_add_projectile(&curstage, this, &bullet, target, 2);    
            this->current_anim->projspawned = false;
        }
        this->lookat = pointangle(this->rotorigin, target) - (FPI/2);

        if (pointdistance(player.pos, this->pos) > 350) {
            vec2 mov = get_vec(this->pos, player.pos);
            mov = unit(mov);

            this->mov = mov;

            this->pos = sum(this->pos, this->mov);
        } else {
            vec2 fodase = {0, 0};
            this->mov = fodase;
        }

        if (this->health <= 0) {
            this->dead = true;
            
            // HACK
            this->current_anim->over = true;
            entity_play_anim(this, "death");

            this->lookat = this->lookat + FPI;
        }
    } else {
        this->deadframes++;

        if (this->deadframes > 480) {
            stage_remove_entity(&curstage, this);
        }
    }
}

static void shotgunner_think(entity *this)
{
    if (!this->dead) {
        entity_play_anim(this, "shot");

        vec2 target = sum(player.pos, player.current_anim->spr.rotcenter);
        float angle = FPI / 12;


        if (this->current_anim->projspawned) {
            stage_add_projectile(&curstage, this, &shell, target, 1.5);    
            stage_add_projectile_ex(&curstage, this, &shell, target, 1.5, angle);    
            stage_add_projectile_ex(&curstage, this, &shell, target, 1.5, -angle);    
            this->current_anim->projspawned = false;
        }
        this->lookat = pointangle(this->rotorigin, target) - (FPI/2);

        if (pointdistance(player.pos, this->pos) > 350) {
            vec2 mov = get_vec(this->pos, player.pos);
            mov = unit(mov);

            this->mov = mov;

            this->pos = sum(this->pos, this->mov);
        } else {
            vec2 fodase = {0, 0};
            this->mov = fodase;
        }

        if (this->health <= 0) {
            this->dead = true;
            
            // HACK
            this->current_anim->over = true;
            entity_play_anim(this, "death");

            this->lookat = this->lookat + FPI;
        }
    } else {
        this->deadframes++;

        if (this->deadframes > 480) {
            stage_remove_entity(&curstage, this);
        }
    }
}

static entity *make_gunslinger(void)
{
    const char *path = "assets/characters/badguy01";

    entity *ent = xmalloc(sizeof(entity));

    memset(ent, 0, sizeof(entity));

    ent->free = &entity_free;
    ent->real_think = &gunslinger_think;
    ent->enemy = true;
    ent->health = 10;

    entity_load(ent, renderer_, path);

    stage_add_entity(&curstage, ent);

    entity_play_anim(ent, "idle");

    return ent;
}

static entity *make_shotgunner(void)
{
    const char *path = "assets/characters/badguy02";

    entity *ent = xmalloc(sizeof(entity));

    memset(ent, 0, sizeof(entity));

    ent->free = &entity_free;
    ent->real_think = &shotgunner_think;
    ent->enemy = true;
    ent->health = 15;

    entity_load(ent, renderer_, path);

    stage_add_entity(&curstage, ent);

    entity_play_anim(ent, "idle");

    return ent;
}

static void spawner_spawn(void)
{
    spawnerarray[0] = make_gunslinger();
    spawnerarray[1] = make_gunslinger();
    spawnerarray[2] = make_gunslinger();
    spawnerarray[3] = make_shotgunner();

    spawnerarray[0]->pos = spawns[0];
    spawnerarray[1]->pos = spawns[1];
    spawnerarray[2]->pos = spawns[2];
    spawnerarray[3]->pos = spawns[3];

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
    player.enemy = false;
    player.real_think = &player_think;
    player.free = &fake_free;
    player.health = PLAYER_HP;

    isbtime = false;
    btimeframes = 240;

    entity_load(&player, renderer, "assets/characters/main");

    entity_play_anim(&player, "revolver");

    stage_load(&curstage, renderer, "assets/background/background_01");

    stage_add_entity(&curstage, &player);

    sprite_load(&bullet, renderer, "assets/weapons/revolver_bullet.png");
    sprite_load(&shell, renderer, "assets/weapons/shotgun_bullet.png");
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
    if (this->health <= 0) {
       // abort();
    }

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
        stage_free(&curstage);
        stage_load(&curstage, renderer_, "assets/background/background_03");
        stage_add_entity(&curstage, &player);
        iptstate.EL = false;
    }

    newmov = unit(newmov);
    this->mov = newmov;

    this->pos = sum(this->pos, this->mov);

    entity_play_anim(this, "revolver");

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
    if (!stage_is_anything_alive(&curstage)) {
        spawner_spawn();
    }

    stage_think(&curstage);
}

void game_paint(SDL_Renderer *renderer, unsigned diff)
{
    stage_paint(&curstage, renderer, diff);

    char buf[10];
    vec2 pos = {100, 645};
    SDL_Color c = {255, 255, 255, SDL_ALPHA_OPAQUE};
    snprintf(buf, 10, "%.0f", player.health / 5);
    ui_button(UI_ID, buf, pos, c);
    SDL_Rect aaaa = {100, 660, player.health * 0.8, 30};

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    int buceta = SDL_RenderFillRect(renderer, &aaaa);
}

void game_quit(void)
{
    stage_free(&curstage);
}

// vim: set ts=4 sw=4 expandtab:
