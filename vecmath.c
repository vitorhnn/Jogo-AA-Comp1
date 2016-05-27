// Copyright © 2016 Victor Hermann "vitorhn" Chiletto
// Licensed under the MIT/Expat license

#include <math.h>
#include "math.h"

static vec2 unit(vec2 *vec) {
    float norm = sqrt(vec->x * vec->x + vec->y * vec->y);

    vec2 retvec = {
        .x = vec->x / norm,
        .y = vec->y / norm
    };

    return retvec;
}

vec2 get_vec(vec2 *a, vec2 *b) {
    vec2 retvec;
    retvec.x = b->x - a->x;
    retvec.y = b->y - a->y;

    return unit(&retvec);
}

// vim: set ts=4 sw=4 expandtab:
