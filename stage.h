// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#ifndef STAGE_H
#define STAGE_H

#include "entity.h"
#include "sprite.h"

typedef struct {
    sprite *spr;
    vec2 pos, mov;
    float angle, damage;
    bool active, enemy, piercing;
} projectile;

typedef struct {
    sprite *spr;
    vec2 pos;
    size_t frames;

    bool active;

    void (*picked_callback)(void);
} pickup;

typedef struct {
    sprite background;
    rect maincol, *colarray;
    struct {
        entity *ent;
        bool active;
    } entwrapper[100];
    projectile projectiles[256];
    pickup pickups[32];
    size_t colc;
} stage;

void stage_load(stage *stage, SDL_Renderer *renderer, const char *path);

void stage_add_entity(stage *stage, entity *ent);

void stage_remove_entity(stage *stage, entity *ent);

void stage_add_pickup(stage *stage, sprite *spr, vec2 pos, void (*callback)(void));

void stage_add_projectile(stage *stage, entity *shooter, sprite *sprm, vec2 target, float speed, float damage);

void stage_add_projectile_ex(stage *stage, entity *shooter, sprite *sprm, vec2 target, float speed, float damage, float angle, bool piercing);

bool stage_is_ent_colliding(stage *stage, entity *ent);

bool stage_is_anything_alive(stage *stage);

void stage_think(stage *stage);

void stage_paint(stage *stage, SDL_Renderer *renderer, unsigned diff);

void stage_free(stage *stage);

#endif
