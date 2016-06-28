// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#ifndef ENTITY_H
#define ENTITY_H

#include "vecmath.h"
#include "anim.h"

typedef struct entity {
    anim  *anims,
          *current_anim;
    size_t animc;
    vec2 pos, mov;
    float lookat;

    void (*think)(struct entity*);
} entity;

void entity_load(entity *ent, SDL_Renderer *renderer, const char *path);

void entity_play_anim(entity *ent, const char *name);

void entity_free(entity *ent);


#endif
