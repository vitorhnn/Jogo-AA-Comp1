// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#ifndef EFFECT_H
#define EFFECT_H

#include "anim.h"

typedef struct {
    anim effect;
    vec2 pos;
    float angle;
    bool active;
} effect;

effect *effect_load(vec2 pos, float angle, const char *path);

void effect_think(effect *fx);

void effect_paint(effect *fx, rect camera);

void effect_free(effect *fx);

#endif
