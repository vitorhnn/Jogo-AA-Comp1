// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#ifndef SPRITE_H
#define SPRITE_H

#include <stdbool.h>

#include <SDL2/SDL.h>

#include "vid.h"
#include "vecmath.h"

typedef struct {
    vid_texture *texture;
    vec2 rotcenter,
         projorigin; // it hurts my soul to add these here, but I can't be arsed to write another struct to wrap these
    int w, h;
} sprite;

bool sprite_load(sprite *sprite, const char *path);

void sprite_paint(sprite *sprite, vec2 pos);

void sprite_paint_less_ex(sprite *sprite, vec2 pos, float angle);

void sprite_paint_ex(sprite *sprite, rect clip, vec2 pos, float angle);

void sprite_free(sprite *sprite);

#endif
