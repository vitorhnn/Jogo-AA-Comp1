// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#ifndef ENTITY_H
#define ENTITY_H

#include "vecmath.h"
#include "anim.h"

typedef struct entity {
    anim  *anims,
          *current_anim,
          staticleg, legs;
    size_t animc, deadframes;
    vec2 pos, mov, rotorigin;
    float lookat, health;
    bool enemy, dead, haslegs;

    void (*real_think)(struct entity *);

    void (*free)(struct entity *);
} entity;

void entity_load(entity *ent, const char *path);

void entity_play_anim(entity *ent, const char *name);

void entity_think(entity *ent);

void entity_paint(entity *ent, rect camera, unsigned diff);

void entity_free(entity *ent);


#endif
