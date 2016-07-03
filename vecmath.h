// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license

#ifndef MATH_H
#define MATH_H

#include <stdbool.h>

typedef struct {
    float x, y;
} vec2;

typedef struct {
    float x, y, w, h;
} rect;

vec2 MAKEVEC(float x, float y);

float pointdistance(vec2 a, vec2 b);

float pointangle(vec2 a, vec2 b);

bool edgecollide(rect a, rect b);

bool fullcollide(rect a, rect b);

vec2 rot_vec(vec2 vec, float angle);

vec2 get_vec(vec2 a, vec2 b);

float norm(vec2 vec);

vec2 unit(vec2 vec);

vec2 sum(vec2 a, vec2 b);

vec2 mul(vec2 vec, float scalar);
#endif
