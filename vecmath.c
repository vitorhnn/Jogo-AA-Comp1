// Copyright © 2016 Victor Hermann "vitorhn" Chiletto
// Licensed under the MIT/Expat license

#include <math.h>
#include "vecmath.h"

vec2 get_vec(vec2 *a, vec2 *b) {
    vec2 retvec;
    retvec.x = b->x - a->x;
    retvec.y = b->y - a->y;

    return unit(&retvec);
}

vec2 unit(vec2 *vec) {
    float norm = sqrtf(vec->x * vec->x + vec->y * vec->y);

    if (norm == 0) {
        return *vec;
    }

    vec2 retvec = {
        .x = vec->x / norm,
        .y = vec->y / norm
    };

    return retvec;
}

vec2 sum(vec2 *a, vec2 *b) {
    vec2 retvec = {
        .x = a->x + b->x,
        .y = a->y + b->y
    };

    return retvec;
}

vec2 mul(vec2 *vec, float scalar) {
    vec2 retvec = {
        .x = vec->x * scalar,
        .y = vec->y * scalar
    };

    return retvec;
}
// vim: set ts=4 sw=4 expandtab:
