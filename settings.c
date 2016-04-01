// Copyright © Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license

// This system is definitely influenced by Quake's cvar system.

#define _XOPEN_SOURCE 500

#include "3rdparty/asprintf/asprintf.h"

#include "common.h"
#include "settings.h"

static setting* last_setting;

void setting_register(setting* setting) {
    // no point in registering if it's already registered
    if (setting_find(setting->name)) {
        return;
    }

    // link it in
    setting->next = last_setting;
    last_setting = setting;

    char* value = strdup(setting->value);
    setting->value = xmalloc(1); // this is needed, as trying to free a constant generates a segfault
    setting_set(setting->name, value);
    free(value);
}

setting* setting_find(char* setting_name) {
    setting* ptr;
    // iterate through each setting, stopping if ->next leads to nowhere (avoids segfaulting!)
    for (ptr = last_setting; ptr != NULL; ptr = ptr->next) {
        if (strcmp(setting_name, ptr->name) == 0) {
            return ptr;
        }
    }
    return NULL;
}

void setting_set(char* setting_name, char* value) {
    setting* setting = setting_find(setting_name);
    if (setting == NULL) {
        show_error("setting_set: attempted to set unregistered setting", ERROR_SOURCE_INTERNAL);
        return;
    }

    free(setting->value);
    setting->value = strdup(value);
}

void setting_set_num(char* setting_name, float value) {
    // convert to a string and pass it along.
    char* valuestr;
    asprintf(&valuestr, "%f", value);
    setting_set(setting_name, valuestr);
}

void setting_set_bool(char* setting_name, bool value) {
    // same deal as above
    char* valuestr;
    valuestr = (value) ? "true" : "false";
    setting_set(setting_name, valuestr);
}

char* setting_strvalue(char* setting_name) {
    setting* setting = setting_find(setting_name);
    if (setting == NULL) {
        show_error("setting_strvalue: attempted to get unregistered setting", ERROR_SOURCE_INTERNAL);
        return NULL;
    }

    return setting->value;
}

float setting_floatvalue(char* setting_name) {
    setting* setting = setting_find(setting_name);
    if (setting == NULL) {
        show_error("setting_floatvalue: attempted to get unregistered setting", ERROR_SOURCE_INTERNAL);
        return 0;
    }

    char* end;
    float maybe_value = strtof(setting->value, &end);

    if (*end) {
       show_error("setting_floatvalue: value was NOT a float.", ERROR_SOURCE_INTERNAL);
       return 0;
    }
    return maybe_value;
}

bool setting_boolvalue(char* setting_name) {
    setting* setting = setting_find(setting_name);
    if (setting == NULL) {
        show_error("setting_boolvalue: attempted to get unregistered setting", ERROR_SOURCE_INTERNAL);
        return 0;
    }

    if (strcmp(setting->value, "true") == 0) {
        return true;
    }
    else if (strcmp(setting->value, "false") == 0) {
        return false;
    }

    show_error("setting_boolvalue: value was NOT a bool", ERROR_SOURCE_INTERNAL);
    return false;
}

void settings_quit(void) {
    setting* ptr;
    for (ptr = last_setting; ptr != NULL; ptr = ptr->next) {
        free(ptr->value);
    }
}

void settings_parse_argv(int argc, char** argv) {
    for (int i = 1; i < argc;) {
        char* current = argv[i];

        // check the first character for a -
        if (current[0] == '-') {
            // parse the setting
            size_t len = strlen(current);
            char* setting_name = xmalloc(len); // no need for -1 here, strlen doesn't count the NULL terminator.
            strncpy(setting_name, current + 1, len);

            setting_set(setting_name, argv[i + 1]);

            i++;
        }
        i++;
    }
}

// vim: set ts=4 sw=4 expandtab:
