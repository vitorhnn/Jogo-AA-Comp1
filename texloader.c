// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#include <SDL2/SDL_image.h>

#include "3rdparty/physicsfs/extras/physfsrwops.h"

#include "common.h"
#include "texloader.h"

SDL_Texture* tex_load_from_file(SDL_Renderer* renderer, const char* path) {
    SDL_RWops* file = PHYSFSRWOPS_openRead(path);
    if (file == NULL) {
        // using ERROR_SOURCE_PHYSICSFS would make more sense, but PHYSICSFS_getLastError
        // resets the current error code, and that's called by PHYSFSRWOPS_openRead
        show_error("tex_load_from_file: PHYSFSRWOPS_openRead failed", ERROR_SOURCE_SDL);
    }
    else {
        SDL_Surface* temp_surf = IMG_Load_RW(file, 0); // there's a IMG_LoadTexture_RW function but that's undocumented 
                                                       // and likely doesn't exist on older versions
        SDL_FreeRW(file);
        if (temp_surf == NULL) {
            show_error("tex_load_from_file: IMG_Load_RW failed", ERROR_SOURCE_SDL);
        }
        else {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, temp_surf);
            SDL_FreeSurface(temp_surf);
            if (tex != NULL) {
                return tex;
            }
        }
    }
    return NULL;
}

// vim: set ts=4 sw=4 expandtab:
