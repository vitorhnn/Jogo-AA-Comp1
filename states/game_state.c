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
    vec2 rawmouse, mousepos;
} iptstate;

enum weapon {
    WEAPON_NONE,
    WEAPON_REVOLVER,
    WEAPON_SHOTGUN,
    WEAPON_KNIFE,
    WEAPON_FISTS
};

enum powerup {
    POWERUP_STRENGTH,
    POWERUP_HEALTH,
    POWERUP_SPEED,
    POWERUP_DAMAGE,
    POWERUP_NONE
};

static enum weapon current_weapon;
static enum powerup current_powerup;

static size_t powerup_frames;

static stage curstage;

static sprite weapon_sprites[sizeof(enum weapon) + 1];
static sprite powerup_sprites[sizeof(enum powerup) + 1];

static sprite frame_back;
static sprite frame_front;

static entity player;
static float btimeframes;
static unsigned score;
static bool isbtime;
static rect camera;
static void player_think(entity *this);

static SDL_Renderer *renderer_; // ugly hack

static Mix_Chunk *revolver_sfx;
static Mix_Chunk *showtime;
static Mix_Chunk *birl;

static entity *spawnerarray[4];
static int waves;
static bool stage_over;

static void fake_free(entity *nothing)
{
#pragma unused (nothing)
}

static void hora_do_show_porra(void)
{
    Mix_PlayChannel(-1, showtime, 0);

    current_weapon = WEAPON_FISTS;
    current_powerup = POWERUP_STRENGTH;
    powerup_frames = 0;
}

static void gib_health_pls(void)
{
    player.health += 50;
}

static void SANIC(void)
{
    if (current_weapon == WEAPON_FISTS) {
        current_weapon = WEAPON_REVOLVER;
    }

    current_powerup = POWERUP_SPEED;
    powerup_frames = 0;
}

static void qaam(void)
{
    if (current_weapon == WEAPON_FISTS) {
        current_weapon = WEAPON_REVOLVER;
    }

    current_powerup = POWERUP_DAMAGE;
    powerup_frames = 0;
}

static void maybe_add_pickup(vec2 where)
{
    float frand = rand() / (float) RAND_MAX;
    frand *= sizeof(enum powerup);

    enum powerup which = roundf(frand);
    
    switch (which) {
        case POWERUP_STRENGTH:
            stage_add_pickup(&curstage, &powerup_sprites[POWERUP_STRENGTH], where, hora_do_show_porra);
            break;
        case POWERUP_HEALTH:
            stage_add_pickup(&curstage, &powerup_sprites[POWERUP_HEALTH], where, gib_health_pls);
            break;
        case POWERUP_SPEED:
            stage_add_pickup(&curstage, &powerup_sprites[POWERUP_SPEED], where, SANIC);
            break;
        case POWERUP_DAMAGE:
            stage_add_pickup(&curstage, &powerup_sprites[POWERUP_DAMAGE], where, qaam);
            break;
    }
}

static void gunslinger_think(entity *this)
{
    if (!this->dead) {
        entity_play_anim(this, "shot");

        vec2 target = sum(player.pos, player.current_anim->spr.rotcenter);
        if (this->current_anim->projspawned) {
            stage_add_projectile(&curstage, this, &weapon_sprites[WEAPON_REVOLVER], target, 5, 2);
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
            score += 5;
        }
    } else {
        this->deadframes++;

        if (this->deadframes > 480) {
            maybe_add_pickup(this->pos);
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
            stage_add_projectile(&curstage, this, &weapon_sprites[WEAPON_SHOTGUN], target, 1.5, 5);
            stage_add_projectile_ex(&curstage, this, &weapon_sprites[WEAPON_SHOTGUN], target, 1.5, 5, angle, false);
            stage_add_projectile_ex(&curstage, this, &weapon_sprites[WEAPON_SHOTGUN], target, 1.5, 5, -angle, false);
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
            score += 10;
        }
    } else {
        this->deadframes++;

        if (this->deadframes > 480) {
            stage_remove_entity(&curstage, this);
        }
    }
}

static void knifethrower_think(entity *this)
{
    if (!this->dead) {
        entity_play_anim(this, "shot");

        vec2 target = sum(player.pos, player.current_anim->spr.rotcenter);
        if (this->current_anim->projspawned) {
            stage_add_projectile_ex(&curstage, this, &weapon_sprites[WEAPON_KNIFE], target, 5, 1, 0, true);
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
            score += 5;
        }
    } else {
        this->deadframes++;

        if (this->deadframes > 480) {
            maybe_add_pickup(this->pos);
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

static entity *make_knifethrower(void)
{
    const char *path = "assets/characters/badguy03";

    entity *ent = xmalloc(sizeof(entity));

    memset(ent, 0, sizeof(entity));

    ent->free = &heap_ent_free;
    ent->real_think = &knifethrower_think;
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
    spawnerarray[2] = make_knifethrower();
    spawnerarray[3] = make_shotgunner();
    
    size_t j = 0;
    for (size_t i = 0; i < ARRAY_SIZE(spawnerarray); ++i) {
        for (; j < curstage.spawnc; ++j) {
            if (stage_is_spawn_visible(curstage.spawns[j], camera)) {
                spawnerarray[i]->pos = curstage.spawns[j];
                j++;
                break;
            } 
        }
    }
    waves--;
}

static void player_init(entity *this)
{
    this->pos = curstage.playerspawn;
    this->mov.x = 0;
    this->mov.y = 0;
    this->enemy = false;
    this->real_think = &player_think;
    this->free = &fake_free;
    this->health = PLAYER_HP;
    this->dead = false;
    this->deadframes = 0;

    score = 0;
    waves = 4;
    stage_over = false;

    current_weapon = WEAPON_SHOTGUN;
    current_powerup = POWERUP_NONE;
    powerup_frames = 0;

    stage_add_entity(&curstage, this);
}


void game_init(SDL_Renderer *renderer)
{
    renderer_ = renderer;
    SDL_RenderSetLogicalSize(renderer, 1280, 720);
    camera.w = 1280;
    camera.h = 720;
    memset(&iptstate, 0, sizeof(iptstate));


    isbtime = false;
    btimeframes = 160;

    entity_load(&player, renderer, "assets/characters/main");

    entity_play_anim(&player, "revolver");

    stage_load(&curstage, renderer, "assets/background/background_01");

    player_init(&player);

    sprite_load(&weapon_sprites[WEAPON_REVOLVER], renderer, "assets/weapons/revolver_bullet.png");
    sprite_load(&weapon_sprites[WEAPON_SHOTGUN], renderer, "assets/weapons/shotgun_bullet.png");
    sprite_load(&weapon_sprites[WEAPON_KNIFE], renderer, "assets/weapons/knife.png");

    sprite_load(&powerup_sprites[POWERUP_STRENGTH], renderer, "assets/powerups/strength.png");
    sprite_load(&powerup_sprites[POWERUP_HEALTH], renderer, "assets/powerups/health.png");
    sprite_load(&powerup_sprites[POWERUP_SPEED], renderer, "assets/powerups/speed.png");
    sprite_load(&powerup_sprites[POWERUP_DAMAGE], renderer, "assets/powerups/damage.png");

    SDL_RWops *fp = PHYSFSRWOPS_openRead("assets/weapons/revolver.ogg");
    revolver_sfx = Mix_LoadWAV_RW(fp, 1);

    fp = PHYSFSRWOPS_openRead("assets/powerups/strength_pickup.ogg");
    showtime = Mix_LoadWAV_RW(fp, 1);

    fp = PHYSFSRWOPS_openRead("assets/powerups/strength_attack.ogg");
    birl = Mix_LoadWAV_RW(fp, 1);

    sprite_load(&frame_back, renderer, "assets/menus/status_bar/status_bar_back.png");
    sprite_load(&frame_front, renderer, "assets/menus/status_bar/status_bar_front.png");

    spawner_spawn();
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
            iptstate.rawmouse.x = event->motion.x;
            iptstate.rawmouse.y = event->motion.y;
            break;
    }
}

static void player_think(entity *this)
{
    vec2 camvec = {
        (player.pos.x + player.current_anim->spr.rotcenter.x / 2) - camera.w / 2,
        (player.pos.y + player.current_anim->spr.rotcenter.y / 2) - camera.h / 2,
    };

    camera.x = camvec.x;
    camera.y = camvec.y;
    
    if (camera.x < 0) {
        camera.x = 0;
    }
    if (camera.y < 0) {
        camera.y = 0;
    }
    if (camera.x > curstage.background.w - camera.w) {
        camera.x = curstage.background.w - camera.w;
    }
    if (camera.y > curstage.background.h - camera.h) {
        camera.y = curstage.background.h - camera.h;
    }

    iptstate.mousepos.x = iptstate.rawmouse.x + camera.x;
    iptstate.mousepos.y = iptstate.rawmouse.y + camera.y;

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

        if (stage_over) {
            rect playercol = {
                this->pos.x,
                this->pos.y,
                this->current_anim->frames[this->current_anim->curframe].w,
                this->current_anim->frames[this->current_anim->curframe].h
            };

            if (fullcollide(playercol, curstage.playerexit)) {
                stage_free(&curstage);
                char *toload = NULL;

                if (strcmp(curstage.name, "assets/background/background_01") == 0) {
                    toload = "assets/background/background_02";
                } else if (strcmp(curstage.name, "assets/background/background_02") == 0) {
                    toload = "assets/background/background_03";
                } else if (strcmp(curstage.name, "assets/background/background_03")) {
                    toload = "assets/background/background_04";
                }

                stage_load(&curstage, renderer_, toload);
                player_init(this);
            }
        }

        if (current_powerup != POWERUP_NONE) {
            powerup_frames++;
        }

        if (powerup_frames >= 2400) {
            powerup_frames = 0;
            
            switch (current_powerup) {
                case POWERUP_STRENGTH:
                    current_weapon = WEAPON_REVOLVER;
                    break;
            }

            current_powerup = POWERUP_NONE;
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

        if (current_powerup == POWERUP_SPEED) {
            this->mov = mul(this->mov, 1.5);
        }

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
            float mod = 1;
            if (current_powerup == POWERUP_DAMAGE) {
                mod = 2; 
            }

            switch (current_weapon) {
                case WEAPON_REVOLVER:
                    stage_add_projectile(&curstage, this, &weapon_sprites[WEAPON_REVOLVER], iptstate.mousepos, 3.5, 5 * mod);
                    break;
                case WEAPON_SHOTGUN: {
                    float angle = FPI / 12;

                    stage_add_projectile(&curstage, this, &weapon_sprites[WEAPON_SHOTGUN], iptstate.mousepos, 1.5, 5 * mod);
                    stage_add_projectile_ex(&curstage, this, &weapon_sprites[WEAPON_SHOTGUN], iptstate.mousepos, 1.5, 5 * mod, angle, false);
                    stage_add_projectile_ex(&curstage, this, &weapon_sprites[WEAPON_SHOTGUN], iptstate.mousepos, 1.5, 5 * mod, -angle, false);

                    break;
                }
                case WEAPON_KNIFE:
                    stage_add_projectile_ex(&curstage, this, &weapon_sprites[WEAPON_KNIFE], iptstate.mousepos, 1.5, 8, 0, true);
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
        if (waves > 0) {
            spawner_spawn();
        } else {
            stage_over = true;
        }
    }

    stage_think(&curstage);
}

void game_paint(SDL_Renderer *renderer, unsigned diff)
{
    stage_paint(&curstage, renderer, camera, diff);

    vec2 frame = {0, 623};
    sprite_paint(&frame_back, renderer, frame);

    SDL_Rect aaaa = {13, 684, player.health * ((float) 337 / PLAYER_HP), 32};
    SDL_Rect saicapeta = {932, 683, btimeframes * ((float) 337 / 160), 32};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &aaaa);
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &saicapeta);

    sprite_paint(&frame_front, renderer, frame);
}

void game_quit(void)
{
    stage_free(&curstage);

    for (size_t i = 0; i < sizeof(enum weapon); ++i) {
        sprite_free(&weapon_sprites[i]);
    }

    for (size_t i = 0; i < sizeof(enum powerup); ++i) {
        sprite_free(&powerup_sprites[i]);
    }
}

// vim: set ts=4 sw=4 expandtab:
