// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#include "common.h"

#include <stdlib.h>

#include <SDL2/SDL.h>
#include <physfs.h>

#include "3rdparty/asprintf/asprintf.h"

void real_show_error(char *msg, error_source source, bool msgbox)
{
    char *message;

    switch (source) {
        case ERROR_SOURCE_SDL:
            asprintf(&message, "%s\nSDL_GetError(): %s", msg, SDL_GetError());
            break;

        case ERROR_SOURCE_PHYSFS:
            asprintf(&message, "%s\nPHYSFS_getLastError(): %s", msg, PHYSFS_getLastError());
            break;

        case ERROR_SOURCE_INTERNAL:
            asprintf(&message, "internal engine error: %s", msg);
            break;

        default:
            asprintf(&message, "show_error_msgbox called with invalid error_source!?");
            break;
    }

    if (message == NULL) {
        // probably OOM, but we still need to print *something* out.
        message = "show_error_msgbox: asprintf failed. you're probably out of memory.";
    }

    if (!msgbox || SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "joguin", message, NULL) != 0) {
        // if the messagebox failed, chances are this is a OOM failure, so fprintf to stderr and stop there.
        fprintf(stderr, "%s\n", message);
    }

    free(message);
}

// xmalloc: malloc that never returns NULL
void *xmalloc(size_t size)
{
    void *block = malloc(size);

    if (block == NULL) {
        // maybe attempt to save the game state in the future?
        // for now though:
        show_error_msgbox("xmalloc: could not allocate more memory", ERROR_SOURCE_INTERNAL);
        abort();
    }

    return block;
}

// vim: set ts=4 sw=4 expandtab:
