// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#include <SDL2/SDL_image.h>

#include "sprite.h"
#include "common.h"
#include "3rdparty/physicsfs/extras/physfsrwops.h"

bool sprite_load(sprite *sprite, const char *path)
{
    SDL_RWops *file = PHYSFSRWOPS_openRead(path);

    if (file == NULL) {
        // using ERROR_SOURCE_PHYSICSFS would make more sense, but PHYSICSFS_getLastError
        // resets the current error code, and that's called by PHYSFSRWOPS_openRead
        show_error("sprite_load: PHYSFSRWOPS_openRead failed", ERROR_SOURCE_SDL);
        return false;
    }

    vid_texture *tex = vid_load_texture(file);

    SDL_FreeRW(file);

    if (tex == NULL) {
        show_error("sprite_load: SDL_CreateTextureFromSurface failed", ERROR_SOURCE_SDL);
        return false;
    }

    sprite->texture = tex;
    sprite->w = tex->width;
    sprite->h = tex->height;

    return true;
}

void sprite_paint(sprite *sprite, vec2 pos)
{
    rect clip = {0, 0, sprite->w, sprite->h};
    sprite_paint_ex(sprite, clip, pos, 0);
}

void sprite_paint_less_ex(sprite *sprite, vec2 pos, float angle)
{
    rect clip = {0, 0, sprite->w, sprite->h};
    sprite_paint_ex(sprite, clip, pos, angle);
}

void sprite_paint_ex(sprite *sprite, rect clip, vec2 pos, float angle)
{
    rect dst = {
        .x = pos.x,
        .y = pos.y,
        .w = clip.w,
        .h = clip.h
    };

    vid_draw_cmd cmd = {
        .texture = sprite->texture,
        .src = clip,
        .dst = dst,
        .angle = angle,
        .center = sprite->rotcenter,
    };

    vid_push_draw_cmd(&cmd);
}

void sprite_free(sprite *sprite)
{
    vid_free_texture(sprite->texture);
}
