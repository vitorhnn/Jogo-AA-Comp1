// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license

#include <stdbool.h>

#include <SDL2/SDL_mixer.h>

#include "../3rdparty/physicsfs/extras/physfsrwops.h"

#include "../settings.h"
#include "../ui.h"
#include "../common.h"
#include "../stage.h"
#include "../main.h"
#include "../entity.h"
#include "../anim.h"
#include "../vecmath.h"
#include "game_state.h"

#define PLAYER_HP 500

static struct {
    bool up, down, left, right, click, EL, btime;
    vec2 mousepos;
} iptstate;

enum weapon {
    WEAPON_NONE,
    WEAPON_REVOLVER,
    WEAPON_SHOTGUN,
    WEAPON_KNIFE,
    WEAPON_FISTS
};

static enum weapon current_weapon;

static entity player;

static stage curstage;

static sprite bullet;

static sprite shell;

static sprite knife;

static sprite strength;

static float btimeframes;

static bool isbtime;

static void player_think(entity *this);

static SDL_Renderer *renderer_; // ugly hack

static Mix_Chunk *revolver_sfx;

static Mix_Chunk *showtime;

static Mix_Chunk *birl;

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

static void gunslinger_think(entity *this)
{
    if (!this->dead) {
        entity_play_anim(this, "shot");

        vec2 target = sum(player.pos, player.current_anim->spr.rotcenter);
        if (this->current_anim->projspawned) {
            stage_add_projectile(&curstage, this, &bullet, target, 5, 2);
            this->current_anim->projspawned = false;

//            Mix_PlayChannel(-1, revolver_sfx, 0);
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
            stage_add_projectile(&curstage, this, &shell, target, 1.5, 5);
            stage_add_projectile_ex(&curstage, this, &shell, target, 1.5, 5, angle, false);
            stage_add_projectile_ex(&curstage, this, &shell, target, 1.5, 5, -angle, false);
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

static void heap_ent_free(entity *this)
{
    free(this);
}

static entity *make_gunslinger(void)
{
    const char *path = "assets/characters/badguy01";

    entity *ent = xmalloc(sizeof(entity));

    memset(ent, 0, sizeof(entity));

    ent->free = &heap_ent_free;
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

    ent->free = &heap_ent_free;
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

static void player_init(entity *this)
{
    this->pos.x = 400;
    this->pos.y = 400;
    this->mov.x = 0;
    this->mov.y = 0;
    this->enemy = false;
    this->real_think = &player_think;
    this->free = &fake_free;
    this->health = PLAYER_HP;
    this->dead = false;
    this->deadframes = 0;

    current_weapon = WEAPON_REVOLVER;
}

static void hora_do_show_porra(void)
{
    Mix_PlayChannel(-1, showtime, 0);

    current_weapon = WEAPON_FISTS;
}

void game_init(SDL_Renderer *renderer)
{
    renderer_ = renderer;
    SDL_RenderSetLogicalSize(renderer, 1280, 720);
    memset(&iptstate, 0, sizeof(iptstate));

    player_init(&player);

    isbtime = false;
    btimeframes = 160;

    entity_load(&player, renderer, "assets/characters/main");

    entity_play_anim(&player, "revolver");

    stage_load(&curstage, renderer, "assets/background/background_01");

    stage_add_entity(&curstage, &player);

    sprite_load(&bullet, renderer, "assets/weapons/revolver_bullet.png");
    sprite_load(&shell, renderer, "assets/weapons/shotgun_bullet.png");
    sprite_load(&knife, renderer, "assets/weapons/knife.png");

    sprite_load(&strength, renderer, "assets/powerups/strenght.png");

    SDL_RWops *fp = PHYSFSRWOPS_openRead("assets/weapons/revolver.ogg");
    revolver_sfx = Mix_LoadWAV_RW(fp, 1);

    fp = PHYSFSRWOPS_openRead("assets/powerups/strength_pickup.ogg");
    showtime = Mix_LoadWAV_RW(fp, 1);

    fp = PHYSFSRWOPS_openRead("assets/powerups/strength_attack.ogg");
    birl = Mix_LoadWAV_RW(fp, 1);

    stage_add_pickup(&curstage, &strength, MAKEVEC(500, 500), hora_do_show_porra);
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

                case SDLK_q:
                    iptstate.btime = true;
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

                case SDLK_q:
                    iptstate.btime = false;
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
    if (!this->dead) {
        if (this->health <= 0) {
            // HACK

            this->dead = true;
            this->current_anim->over = true;
            entity_play_anim(this, "death");

            this->lookat = this->lookat + FPI;

            return;
        }

        if (iptstate.btime) {
            iptstate.btime = false;

            isbtime = !isbtime;
        }

        if (isbtime && btimeframes > 0) {
            setting_set_num("game_tickrate", 40);
            btimeframes--;
        } else {
            isbtime = false;
            setting_set_num("game_tickrate", 240);

            if (btimeframes < 160) {
                btimeframes += 0.1;
            }
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

            return;
        }

        newmov = unit(newmov);
        this->mov = newmov;

        this->pos = sum(this->pos, this->mov);

        switch (current_weapon) {
            case WEAPON_REVOLVER:
                entity_play_anim(this, "revolver");
                break;
            case WEAPON_SHOTGUN:
                entity_play_anim(this, "shotgun");
                break;
            case WEAPON_KNIFE:
                entity_play_anim(this, "knife");
                break;
            case WEAPON_FISTS:
                entity_play_anim(this, "melee");
        }

        if (stage_is_ent_colliding(&curstage, this)) {
            vec2 unmov = this->mov;
            unmov.x = -unmov.x;
            unmov.y = -unmov.y;

            this->pos = sum(this->pos, unmov);
        }

        if (iptstate.click) {
            switch (current_weapon) {
                case WEAPON_REVOLVER:
                    entity_play_anim(this, "revolver_shot");
                    break;
                case WEAPON_SHOTGUN:
                    entity_play_anim(this, "shotgun_shot");
                    break;
                case WEAPON_KNIFE:
                    entity_play_anim(this, "knife_shot");
                    break;
                case WEAPON_FISTS:
                    entity_play_anim(this, "melee_attack");
            }

            iptstate.click = false;
        }

        if (this->current_anim->projspawned) {
            switch (current_weapon) {
                case WEAPON_REVOLVER:
                    stage_add_projectile(&curstage, this, &bullet, iptstate.mousepos, 3.5, 5);
                    break;
                case WEAPON_SHOTGUN: {
                    float angle = FPI / 12;

                    stage_add_projectile(&curstage, this, &shell, iptstate.mousepos, 1.5, 5);
                    stage_add_projectile_ex(&curstage, this, &shell, iptstate.mousepos, 1.5, 5, angle, false);
                    stage_add_projectile_ex(&curstage, this, &shell, iptstate.mousepos, 1.5, 5, -angle, false);

                    break;
                }
                case WEAPON_KNIFE:
                    stage_add_projectile_ex(&curstage, this, &knife, iptstate.mousepos, 1.5, 8, 0, true);
                    break;
                case WEAPON_FISTS:
                    Mix_PlayChannel(-1, birl, 0);
            }

            this->current_anim->projspawned = false;
        }

        this->lookat = pointangle(this->rotorigin, iptstate.mousepos) - (FPI / 2);
    } else {
        this->deadframes++;

        if (this->deadframes > 960) {
            engine_switch_state(STATE_MENU);
        }
    }
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
    SDL_Rect saicapeta = {850, 660, btimeframes * 2.5, 30};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &aaaa);
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &saicapeta);
}

void game_quit(void)
{
    stage_free(&curstage);
}

// vim: set ts=4 sw=4 expandtab:
