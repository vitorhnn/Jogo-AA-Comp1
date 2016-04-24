// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#ifndef VIDEO_H
#define VIDEO_H

#include <stdint.h>

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} vid_color;

typedef struct {
    void *driver_data;
} vid_texture;

void vid_init(void);

void vid_quit(void);

void vid_set_window_title(const char *title);

void vid_set_draw_color(const vid_color color);

void vid_set_logical_size(int width, int height);

void vid_clear(void);

void vid_present(void);

#endif
