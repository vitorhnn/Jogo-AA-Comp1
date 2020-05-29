// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#ifndef VIDEO_H
#define VIDEO_H

#include <stdint.h>
#include <SDL2/SDL_rwops.h>

#include "vecmath.h"

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} vid_color;

typedef struct {
    void *driver_data;
    int width;
    int height;
} vid_texture;

typedef struct {
    vid_texture *texture;
    rect src;
    rect dst;
    float angle;
    vec2 center;
} vid_draw_cmd;

void vid_init(void);

void vid_quit(void);

void vid_set_draw_color(const vid_color color);

void vid_set_logical_size(int width, int height);

vid_texture *vid_load_texture(SDL_RWops *rw);

void vid_free_texture(vid_texture *texture);

void vid_push_draw_cmd(vid_draw_cmd *cmd);

void vid_clear(void);

void vid_present(void);

#endif
