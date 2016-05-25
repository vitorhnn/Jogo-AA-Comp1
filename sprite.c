// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#include <SDL2/SDL_image.h>

#include "sprite.h"
#include "common.h"
#include "3rdparty/physicsfs/extras/physfsrwops.h"

bool sprite_load(sprite *sprite, SDL_Renderer *renderer, const char *path) {
    SDL_RWops *file = PHYSFSRWOPS_openRead(path);

    if (file == NULL) {
        // using ERROR_SOURCE_PHYSICSFS would make more sense, but PHYSICSFS_getLastError
        // resets the current error code, and that's called by PHYSFSRWOPS_openRead
        show_error("sprite_load: PHYSFSRWOPS_openRead failed", ERROR_SOURCE_SDL);
    }
    else {
        SDL_Surface *temp_surf = IMG_Load_RW(file, 1);

        if (temp_surf == NULL) {
            show_error("sprite_load: IMG_Load_RW failed", ERROR_SOURCE_SDL);
        }
        else {
            SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, temp_surf);
 
            int w = temp_surf->w,
                h = temp_surf->h;

            SDL_FreeSurface(temp_surf);
            
            if (tex == NULL) {
                show_error("sprite_load: SDL_CreateTextureFromSurface failed", ERROR_SOURCE_SDL);
            }
            else {
                sprite->texture = tex;
                sprite->w = w;
                sprite->h = h;

                return true;
            }
        }
    }
    return false;
}

void sprite_paint(sprite *sprite, SDL_Renderer* renderer, vec2i pos) {
    SDL_Rect rect = {
        .x =  pos.x,
        .y =  pos.y,
        .w = sprite->w,
        .h = sprite->h
    };
    SDL_RenderCopy(renderer, sprite->texture, NULL, &rect);
}

void sprite_free(sprite *sprite) {
    SDL_DestroyTexture(sprite->texture);
}
