// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#ifndef COMMON_H
#define COMMON_H

#define FPI (acosf(-1))

#include <stddef.h>
#include <stdbool.h>

typedef enum {ERROR_SOURCE_SDL, ERROR_SOURCE_PHYSFS, ERROR_SOURCE_INTERNAL} error_source;

typedef enum {
    STATE_CREDITS,
    STATE_MENU,
    STATE_GAME
} game_state;

void real_show_error(char *msg, error_source source, bool show_messagebox);

#define show_error(msg, source) real_show_error(msg, source, false)
#define show_error_msgbox(msg, source) real_show_error(msg, source, true)


void *xmalloc(size_t size);

#endif

// vim: set ts=4 sw=4 expandtab:
