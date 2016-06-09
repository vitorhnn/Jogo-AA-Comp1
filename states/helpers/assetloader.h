// Copyright © 2016 Victor Hermann "vitorhn" Chiletto
// Licensed under the MIT/Expat license

#ifndef ASSETLOADER_H
#define ASSETLOADER_H

#include "../game_state.h"

sprite sheet_load(SDL_Renderer *renderer, const char *path);

void background_load(SDL_Renderer *renderer, const char *path, struct background *bg);

void entity_load(SDL_Renderer *renderer, const char *path, struct entity *ent);

#endif
