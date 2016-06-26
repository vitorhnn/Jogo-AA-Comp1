// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#include <SDL2/SDL_image.h>

#include "sprite.h"
#include "common.h"
#include "3rdparty/physicsfs/extras/physfsrwops.h"

bool sprite_load(sprite *sprite, SDL_Renderer *renderer, const char *path)
{
    SDL_RWops *file = PHYSFSRWOPS_openRead(path);

    if (file == NULL) {
        // using ERROR_SOURCE_PHYSICSFS would make more sense, but PHYSICSFS_getLastError
        // resets the current error code, and that's called by PHYSFSRWOPS_openRead
        show_error("sprite_load: PHYSFSRWOPS_openRead failed", ERROR_SOURCE_SDL);
    } else {
        SDL_Surface *temp_surf = IMG_Load_RW(file, 1);

        if (temp_surf == NULL) {
            show_error("sprite_load: IMG_Load_RW failed", ERROR_SOURCE_SDL);
        } else {
            SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, temp_surf);

            int w = temp_surf->w,
                h = temp_surf->h;

            SDL_FreeSurface(temp_surf);

            if (tex == NULL) {
                show_error("sprite_load: SDL_CreateTextureFromSurface failed", ERROR_SOURCE_SDL);
            } else {
                sprite->texture = tex;
                sprite->w = w;
                sprite->h = h;

                return true;
            }
        }
    }

    return false;
}

void sprite_paint(sprite *sprite, SDL_Renderer *renderer, vec2 pos)
{
    rect clip = {0, 0, sprite->w, sprite->h};
    sprite_paint_ex(sprite, renderer, clip, pos, 0);
}

void sprite_paint_less_ex(sprite *sprite, SDL_Renderer *renderer, vec2 pos, float angle)
{
    rect clip = {0, 0, sprite->w, sprite->h};
    sprite_paint_ex(sprite, renderer, clip, pos, angle);
}

void sprite_paint_ex(sprite *sprite, SDL_Renderer *renderer, rect clip, vec2 pos, float angle)
{
    SDL_Rect sdlclip = {
        .x = (int) clip.x,
        .y = (int) clip.y,
        .w = (int) clip.w,
        .h = (int) clip.h
    };

    SDL_Rect rect = {
        .x = (int) pos.x,
        .y = (int) pos.y,
        .w = (int) clip.w,
        .h = (int) clip.h
    };

    SDL_Point point = {
        .x = (int) sprite->rotcenter.x,
        .y = (int) sprite->rotcenter.y
    };

    // SDL uses degrees for whatever god forsaken reason
    double degrees = (angle * (180 / acos(-1)));
    SDL_RenderCopyEx(renderer, sprite->texture, &sdlclip, &rect, degrees, &point, SDL_FLIP_NONE);
}
void sprite_free(sprite *sprite)
{
    SDL_DestroyTexture(sprite->texture);
}
