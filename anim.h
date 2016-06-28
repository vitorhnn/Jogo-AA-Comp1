// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#ifndef ANIM_H
#define ANIM_H

#include "sprite.h"

typedef struct {
    char name[64];
    sprite spr;
    rect *frames;
    size_t  framec,
            curframe,
            origframe,
            logicalframe;
    bool over, projspawned, once;
} anim;

void anim_load(anim *anim, SDL_Renderer *renderer, const char *path, const char *name);

void anim_think(anim *anim);

void anim_paint(anim *anim, SDL_Renderer *renderer, vec2 pos, float angle);

void anim_free(anim* anim);

#endif
