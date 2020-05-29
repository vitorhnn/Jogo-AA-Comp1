// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#include "common.h"

#include "effect.h"


effect *effect_load(vec2 pos, float angle, const char *path)
{
    effect *fx = xmalloc(sizeof(effect));

    memset(fx, 0, sizeof(effect));
    
    fx->pos = pos;
    fx->active = true;
    fx->angle = angle;

    anim_load(&fx->effect, path, "effect");

    return fx;
}

void effect_think(effect *fx)
{
    anim_think(&fx->effect);
    if (fx->effect.over) {
        fx->active = false;
        //effect_free(fx);
    }
}

void effect_paint(effect *fx, rect camera)
{
    vec2 pos = {fx->pos.x - camera.x, fx->pos.y - camera.y};
    anim_paint(&fx->effect, pos, fx->angle); 
}

void effect_free(effect *fx)
{
    anim_free(&fx->effect);
    free(fx);
}
