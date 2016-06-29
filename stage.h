// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#ifndef STAGE_H
#define STAGE_H

#include "entity.h"
#include "sprite.h"

typedef struct {
    sprite *spr;
    vec2 pos, mov;
    float angle;
    bool active;
} projectile;

typedef struct {
    sprite background;
    rect maincol, *colarray;
    struct {
        entity *ent;
        bool active;
    } entwrapper[100];
    projectile projectiles[256];
    size_t colc;
} stage;

void stage_load(stage *stage, SDL_Renderer *renderer, const char *path);

void stage_add_entity(stage *stage, entity *ent);

void stage_add_projectile(stage *stage, entity *shooter, sprite *sprm, vec2 target, float speed);

bool stage_is_ent_colliding(stage *stage, entity *ent);

void stage_think(stage *stage);

void stage_paint(stage *stage, SDL_Renderer *renderer, unsigned diff);

void stage_free(stage *stage);

#endif
