// Copyright © 2016 Victor Hermann "vitorhn" Chiletto
// Licensed under the MIT/Expat license

#define _XOPEN_SOURCE

#include <physfs.h>
#include <stdio.h>
#include "../../3rdparty/asprintf/asprintf.h"
#include "../../sprite.h"
#include "../../common.h"
#include "assetloader.h"

void background_load(SDL_Renderer *renderer, const char *bgname, struct background *bg)
{
    char *mnftpath;
    asprintf(&mnftpath, "%s.manifest", bgname);
    PHYSFS_file *fp = PHYSFS_openRead(mnftpath);

    if (fp == NULL) {
        char err[128];
        snprintf(err, 128, "couldn't load %s", mnftpath);

        show_error(err, ERROR_SOURCE_PHYSFS);

        free(mnftpath);
        return;
    }

    free(mnftpath);


    char buf[1024] = {0};
    PHYSFS_sint64 read = 0;

    while ((read = PHYSFS_read(fp, buf, sizeof(char), 1024)) > 0) {
        char *tok = strtok(buf, "\n");

        if (strncmp(tok, "BACKGROUND", 10) != 0) {
            // invalid magic, stop
            return;
        }

        tok = strtok(NULL, "\n");

        bool is_main_col = true;

        while (tok) {
            if (strncmp(tok, "col", 3) == 0) {
                is_main_col = false;
            }
            if (is_main_col) {
                if (strncmp(tok, "x:", 2) == 0) {
                    tok += 2;
                    // TODO: (will probably never be done!): check (float) strtol's success
                    bg->col.x = (float) (strtol(tok, NULL, 10));
                } else if (strncmp(tok, "y:", 2) == 0) {
                    tok += 2;

                    bg->col.y = (float) (strtol(tok, NULL, 10));
                } else if (strncmp(tok, "w:", 2) == 0) {
                    tok += 2;

                    bg->col.w = (float) (strtol(tok, NULL, 10));
                } else if (strncmp(tok, "h:", 2) == 0) {
                    tok += 2;

                    bg->col.h = (float) (strtol(tok, NULL, 10));
                }
            }

            tok = strtok(NULL, "\n");
        }
    }

    PHYSFS_close(fp);

    char *sprpath;
    asprintf(&sprpath, "%s.png", bgname);
    sprite_load(&(bg->spr), renderer, sprpath);
    free(sprpath);
}

