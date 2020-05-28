// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license

#ifndef MATH_H
#define MATH_H

#include <math.h>
#include <stdbool.h>

typedef struct {
    float x, y, pad1, pad2;
} vec2;

typedef struct {
    float x, y, w, h;
} rect;

static vec2 MAKEVEC(float x, float y)
{
    vec2 r = {x, y};

    return r;
}

// not really vector math, but I need this, so
static float pointdistance(vec2 a, vec2 b)
{
    float x = powf(b.x - a.x, 2);
    float y = powf(b.y - a.y, 2);

    return sqrtf(x + y);
}

static float pointangle(vec2 a, vec2 b)
{
    float deltaX = a.x - b.x;
    float deltaY = a.y - b.y;

    return atan2f(deltaY, deltaX);
}

static bool edgecollide(rect a, rect b)
{
    // totally not copy and pasted from my old pong clone
    if (a.y + a.h <= b.y) {
        return false;
    }

    if (a.y >= b.y + b.h) {
        return false;
    }

    if (a.x + a.w <= b.x) {
        return false;
    }

    if (a.x >= b.x + b.w) {
        return false;
    }

    return true;
}

static bool fullcollide(rect a, rect b)
{
    if (a.y + a.h < b.y) {
        return false;
    }

    if (a.y > b.y + b.h) {
        return false;
    }

    if (a.x + a.w < b.x) {
        return false;
    }

    if (a.x > b.x + b.w) {
        return false;
    }

    return true;
}

static vec2 rot_vec(vec2 vec, float angle)
{
    vec2 retvec = {
        .x = vec.x * cosf(angle) - vec.y * sinf(angle),
        .y = vec.x * sinf(angle) + vec.y * cosf(angle)
    };

    return retvec;
}

static float norm(vec2 vec)
{
    return sqrtf(vec.x * vec.x + vec.y * vec.y);
}

static vec2 unit(vec2 vec)
{
    float n = norm(vec);

    if (n == 0) {
        return vec;
    }

    vec2 retvec = {
        .x = vec.x / n,
        .y = vec.y / n
    };

    return retvec;
}

static vec2 get_vec(vec2 a, vec2 b)
{
    vec2 retvec;
    retvec.x = b.x - a.x;
    retvec.y = b.y - a.y;

    return unit(retvec);
}


static vec2 sum(vec2 a, vec2 b)
{
    vec2 retvec = {
        .x = a.x + b.x,
        .y = a.y + b.y
    };

    return retvec;
}

static vec2 mul(vec2 vec, float scalar)
{
    vec2 retvec = {
        .x = vec.x * scalar,
        .y = vec.y * scalar
    };

    return retvec;
}

#endif
