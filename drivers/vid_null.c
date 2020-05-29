// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

// null driver: does absolutely nothing
// might be useful for building dedicated servers, if this game ever goes multiplayer

#include "../vid.h"
#include "../common.h"

#warning ("Compiling null video driver, this is probably not what you want.")

static vid_texture dummy = {
    NULL,
    0,
    0
};

void vid_init(void)
{
}

void vid_quit(void)
{
}

void vid_set_draw_color(const vid_color color)
{
#pragma unused (color)
}

void vid_set_logical_size(int width, int height)
{
#pragma unused (width, height)
}

vid_texture *vid_load_texture(SDL_RWops *rw)
{
    return &dummy;
}

void vid_free_texture(vid_texture *texture)
{
}

void vid_push_draw_cmd(vid_draw_cmd *cmd)
{
}

void vid_clear(void)
{
}

void vid_present(void)
{
}
