// Copyright © Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/X11
#ifndef TEXLOADER_H
#define TEXLOADER_H

#include <SDL2/SDL.h>

SDL_Texture* tex_load_from_file(SDL_Renderer* renderer, const char* path);

#endif
