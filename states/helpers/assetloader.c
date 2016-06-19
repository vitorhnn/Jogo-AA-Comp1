// Copyright © 2016 Victor Hermann "vitorhn" Chiletto
// Licensed under the MIT/Expat license

#define _XOPEN_SOURCE

#include <physfs.h>
#include <stdio.h>
#include "../../3rdparty/asprintf/asprintf.h"
#include "../../sprite.h"
#include "../../common.h"
#include "assetloader.h"

sprite sheet_load(SDL_Renderer *renderer, const char *path)
{
    sprite retspr;
    // first, load it as if it were a normal sprite:
    char *pngpath;
    asprintf(&pngpath, "%s.png", path);

    sprite_load(&retspr, renderer, pngpath);

    free(pngpath);
    // change the width and height to values defined in the manifest

    char *mnftpath;
    asprintf(&mnftpath, "%s.manifest", path);

    PHYSFS_file *fp = PHYSFS_openRead(mnftpath);

    if (fp == NULL) {
        char err[128];
        snprintf(err, 128, "couldn't load %s", mnftpath);

        show_error(err, ERROR_SOURCE_PHYSFS);
        free(mnftpath);

        return retspr;
    }
    free(mnftpath);

    char buf[1024] = {0};
    PHYSFS_sint64 read = 0;

    while ((read = PHYSFS_read(fp, buf, sizeof(char), 1024)) > 0) {
        char *line = strtok(buf, "\n");

        if (strncmp(line, "SPRITE", 6) != 0) {
            // we should handle failure here.
            // but really, let's just crash.

            abort();
        }

        line = strtok(NULL, "\n");

        while (line) {
            if (strncmp(line, "n:", 2) == 0) {
                line += 2;

                retspr.frames = (int) strtol(line, NULL, 10);
            } else if (strncmp(line, "w:", 2) == 0) {
                line += 2;

                retspr.w = (int) strtol(line, NULL, 10);
            } else if (strncmp(line, "h:", 2) == 0) {
                line += 2;

                retspr.w = (int) strtol(line, NULL, 10);
            } else if (strncmp(line, "rot:", 4) == 0) {
                line += 4;

                vec2 center;

                char x[128] = {0};
                for (int i = 0; *line != ','; i++, line++) {
                    x[i] = *line;
                }
                line++;

                center.x = strtof(x, NULL);
                center.y = strtof(line, NULL);

                retspr.rotcenter = center;

            } else if (strncmp(line, "proj:", 5) == 0) {
                line += 5;

                vec2 origin;

                char x[128] = {0};
                for (int i = 0; *line != ','; i++, line++) {
                    x[i] = *line;
                }
                line++;

                origin.x = strtof(x, NULL);
                origin.y = strtof(line, NULL);

                retspr.projorigin = origin;
            }
            line = strtok(NULL, "\n");
        }
    }
    PHYSFS_close(fp);

    return retspr;
}
void entity_load(SDL_Renderer *renderer, const char *entname, struct entity *ent)
{
    char *path;
    asprintf(&path, "%s/entity.manifest", entname);
    PHYSFS_file *fp = PHYSFS_openRead(path);

    if (fp == NULL) {
        char err[128];
        snprintf(err, 128, "couldn't load %s", path);

        show_error(err, ERROR_SOURCE_PHYSFS);
        free(path);

        return;
    }
    free(path);

    char buf[1024] = {0};
    PHYSFS_sint64 read = 0;

    while ((read = PHYSFS_read(fp, buf, sizeof(char), 1024)) > 0) {
        char *saveptr;
        char *line = strtok_r(buf, "\n", &saveptr);

        if (strncmp(line, "ENTITY", 6) != 0) {
            abort();
        }

        line = strtok_r(NULL, "\n", &saveptr);
        bool parsing_anims = false;
        while (line) {
            if (strncmp(line, "anims:", 6) == 0) {
                parsing_anims = true;
            } else if (strncmp(line, "idle", 4) == 0 && parsing_anims) {
                char *sheetpath;
                asprintf(&sheetpath, "%s/idle", entname);
                ent->idle = sheet_load(renderer, sheetpath);
                free(sheetpath);
            } else if (strncmp(line, "revolver", 8) == 0 && parsing_anims) {
                char *sheetpath;
                asprintf(&sheetpath, "%s/revolver", entname);
                ent->revolver = sheet_load(renderer, sheetpath);
                free(sheetpath);
            } else if (strncmp(line, "revolver_shot", 13) == 0 && parsing_anims) {
                char *sheetpath;
                asprintf(&sheetpath, "%s/revolver_shot", entname);
                ent->revolver_shot = sheet_load(renderer, sheetpath);
                free(sheetpath);
            } else if (strncmp(line, "death", 5) == 0 && parsing_anims) {
                char *sheetpath;
                asprintf(&sheetpath, "%s/death", entname);
                ent->death = sheet_load(renderer, sheetpath);
                free(sheetpath);
            }

            line = strtok_r(NULL, "\n", &saveptr);
        }
    }
    PHYSFS_close(fp);
}


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
        while (tok) {
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
            tok = strtok(NULL, "\n");
        }
    }
    PHYSFS_close(fp);

    char *sprpath;
    asprintf(&sprpath, "%s.png", bgname);
    sprite_load(&(bg->spr), renderer, sprpath);
    free(sprpath);
}

/*
void entity_load(const char *path, struct entity *ent) {
    PHYSFS_file *fp = PHYSFS_openRead(path);

    char buf[1024] = {0};
    PHYSFS_sint64 read = 0;
    while ((read = PHYSFS_read(fp, buf, sizeof(char), 1024)) > 0) {
        char *tok = strtok(buf, "\n");

        while (tok) {
            if (strncmp(tok, "rot", 3) == 0) {
                while (*tok != ':') {
                    tok++;
                }
                tok++;

                vec2 center;

                char x[128];
                for (int i = 0; *tok != ','; i++, tok++) {
                    x[i] = *tok;
                }
                tok++;

                center.x = strtof(x, NULL);
                center.y = strtof(tok, NULL);

                ent->rotcenter = center;

                tok = strtok(NULL, "\n");
            }
        }
    }
}
*/
