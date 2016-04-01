// Copyright © Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdbool.h>
#include <SDL2/SDL_render.h>

typedef struct setting {
    char* name;
    char* value;
    struct setting* next;
} setting;

void setting_register(setting* setting);

setting* setting_find(char* setting_name);

void setting_set(char* setting_name, char* value);

void setting_set_num(char* setting_name, float value);

void setting_set_bool(char* setting_name, bool value);

char* setting_strvalue(char* setting_name);

float setting_floatvalue(char* setting_name);

bool setting_boolvalue(char* setting_name);

void settings_quit(void);

void settings_parse_argv(int argc, char** argv);

#endif

//vim: set ts=4 sw=4 expandtab:
