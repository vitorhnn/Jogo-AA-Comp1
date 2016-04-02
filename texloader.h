// Copyright © Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat
#ifndef TEXLOADER_H
#define TEXLOADER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

SDL_Texture* tex_load_from_file(SDL_Renderer* renderer, const char* path);

SDL_Texture* tex_from_text(SDL_Renderer* renderer, const char* text, TTF_Font* font, SDL_Color color, int* width, int* height);

#endif

// vim: set ts=4 sw=4 expandtab:
