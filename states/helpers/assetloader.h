// Copyright © 2016 Victor Hermann "vitorhn" Chiletto
// Licensed under the MIT/Expat license

#ifndef ASSETLOADER_H
#define ASSETLOADER_H

#include "../game_state.h"

void background_load(const char *path, struct background *bg);

void entity_load(const char *path, struct entity *ent);
#endif
