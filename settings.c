// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license

// This system is definitely influenced by Quake's cvar system.

#define _XOPEN_SOURCE 500

#include <stdlib.h>
#include <string.h>

#include "3rdparty/asprintf/asprintf.h"

#include "common.h"
#include "settings.h"

static setting *last_setting;

void setting_register(setting *setting)
{
    // no point in registering if it's already registered
    if (setting_find(setting->name)) {
        return;
    }

    // link it in
    setting->next = last_setting;
    last_setting = setting;
}

setting *setting_find(char *setting_name)
{
    setting *ptr;

    // iterate through each setting, stopping if ->next leads to nowhere (avoids segfaulting!)
    for (ptr = last_setting; ptr != NULL; ptr = ptr->next) {
        if (strcmp(setting_name, ptr->name) == 0) {
            return ptr;
        }
    }

    return NULL;
}

void setting_set(char *setting_name, char *value)
{
    setting *setting = setting_find(setting_name);

    if (setting == NULL) {
        show_error("setting_set: attempted to set unregistered setting", ERROR_SOURCE_INTERNAL);
        return;
    }

    if (setting->type != SETTING_STR) {
        show_error("setting_set: attempted to set non str setting", ERROR_SOURCE_INTERNAL);
        return;
    }

    strncpy(setting->value.strval, value, sizeof(setting->value.strval));
    setting->value.strval[sizeof(setting->value.strval) - 1] = '\0';
}

void setting_set_float(char *setting_name, float value)
{
    setting *setting = setting_find(setting_name);

    if (setting == NULL) {
        show_error("setting_set: attempted to set unregistered setting", ERROR_SOURCE_INTERNAL);
        return;
    }

    if (setting->type != SETTING_FLOAT) {
        show_error("setting_set_float: attempted to set non float setting", ERROR_SOURCE_INTERNAL);
        return;
    }

    setting->value.floatval = value;
}

void setting_set_bool(char *setting_name, bool value)
{
    // same deal as above
    setting *setting = setting_find(setting_name);

    if (setting == NULL) {
        show_error("setting_set: attempted to set unregistered setting", ERROR_SOURCE_INTERNAL);
        return;
    }

    if (setting->type != SETTING_BOOL) {
        show_error("setting_set_bool: attempted to set non bool setting", ERROR_SOURCE_INTERNAL);
        return;
    }

    setting->value.boolval = value;
}

char *setting_strvalue(char *setting_name)
{
    setting *setting = setting_find(setting_name);

    if (setting == NULL) {
        show_error("setting_strvalue: attempted to get unregistered setting", ERROR_SOURCE_INTERNAL);
        return NULL;
    }

    if (setting->type != SETTING_STR) {
        show_error("setting_strvalue: attempted to get non str setting", ERROR_SOURCE_INTERNAL);
        return NULL;
    }

    return setting->value.strval;
}

float setting_floatvalue(char *setting_name)
{
    setting *setting = setting_find(setting_name);

    if (setting == NULL) {
        show_error("setting_floatvalue: attempted to get unregistered setting", ERROR_SOURCE_INTERNAL);
        return 0;
    }

    if (setting->type != SETTING_FLOAT) {
        show_error("setting_floatvalue: attempted to get non float setting", ERROR_SOURCE_INTERNAL);
        return -1.0f;
    }

    return setting->value.floatval;
}

int setting_intvalue(char *setting_name)
{
    setting *setting = setting_find(setting_name);

    if (setting == NULL) {
        show_error("setting_intvalue: attempted to get unregistered setting", ERROR_SOURCE_INTERNAL);
        return 0;
    }

    if (setting->type != SETTING_INT) {
        show_error("setting_intvalue: attempted to get non int setting", ERROR_SOURCE_INTERNAL);
        return -1;
    }

    return setting->value.intval;
}

bool setting_boolvalue(char *setting_name)
{
    setting *setting = setting_find(setting_name);

    if (setting == NULL) {
        show_error("setting_boolvalue: attempted to get unregistered setting", ERROR_SOURCE_INTERNAL);
        return 0;
    }

    if (setting->type != SETTING_BOOL) {
        show_error("setting_boolvalue: attempted to get non bool setting", ERROR_SOURCE_INTERNAL);
        return false;
    }

    return setting->value.boolval;
}

void settings_quit(void)
{
}

void settings_parse_argv(int argc, char **argv)
{
    for (int i = 1; i < argc; i++) {
        char *current = argv[i];

        // check the first character for a -
        if (current[0] == '-') {
            // parse the setting
            size_t len = strlen(current);
            char *setting_name = xmalloc(len); // no need for -1 here, strlen doesn't count the NULL terminator.
            strncpy(setting_name, current + 1, len);

            if (i + 1 < argc) {
                setting_set(setting_name, argv[i + 1]);
            }

            free(setting_name);
            i++;
        }
    }
}

// vim: set ts=4 sw=4 expandtab:
