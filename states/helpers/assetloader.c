// Copyright © 2016 Victor Hermann "vitorhn" Chiletto
// Licensed under the MIT/Expat license

#include <physfs.h>
#include "assetloader.h"

void background_load(const char *path, struct background *bg) {
    PHYSFS_file *fp = PHYSFS_openRead(path);
    
    char buf[1024] = {0};
    PHYSFS_sint64 read = 0;
    while ((read = PHYSFS_read(fp, buf, sizeof(char), 1024)) > 0) {
        char *tok = strtok(buf, "\n");

        if (strncmp(tok, "bg", 2) != 0) {
            // invalid magic, stop
            return;
        }
        tok = strtok(NULL, "\n");
        while (tok) {
            if (*tok == 'x') {
                while (*tok != ':') {
                    tok++;
                }
                tok++;
                // TODO: (will probably never be done!): check (float) strtol's success
                bg->col.x = (float) (strtol(tok, NULL, 10));
            }
            else if (*tok == 'y') {
                while (*tok != ':') {
                    tok++;
                }
                tok++;

                bg->col.y = (float) (strtol(tok, NULL, 10));
            }
            else if (*tok == 'w') {
                while (*tok != ':') {
                    tok++;
                }
                tok++;

                bg->col.w = (float) (strtol(tok, NULL, 10));
            }
            else if (*tok == 'h') {
                while (*tok != ':') {
                    tok++;
                }
                tok++;

                bg->col.h = (float) (strtol(tok, NULL, 10));
            }
            tok = strtok(NULL, "\n");
        }
    }
}

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
