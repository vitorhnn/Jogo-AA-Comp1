// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#ifndef SPRITE_H
#define SPRITE_H

#include <stdbool.h>

#include <SDL2/SDL.h>

#include "vecmath.h"

typedef struct {
    SDL_Texture *texture;
    vec2 rotcenter;
    int w, h, frames;
} sprite;

bool sprite_load(sprite *sprite, SDL_Renderer *renderer, const char *path);

void sprite_paint(sprite *sprite, SDL_Renderer *renderer, vec2 pos);

void sprite_paint_ex(sprite *sprite, SDL_Renderer *renderer, vec2 pos, float angle, vec2 rotcenter);

void sprite_free(sprite *sprite);

#endif
