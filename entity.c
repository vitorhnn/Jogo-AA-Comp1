// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#include <physfs.h>

#include "3rdparty/parson/parson.h"

#include "common.h"
#include "entity.h"

void entity_load(entity *ent, SDL_Renderer *renderer, const char *path)
{
    char jsonpath[256];
    snprintf(jsonpath, 256, "%s/entity.json", path);

    PHYSFS_file *fp = PHYSFS_openRead(jsonpath);

    if (fp == NULL) {
        show_error_msgbox("entity_load failed, i'm fucking tired of writing error messages", ERROR_SOURCE_PHYSFS);
        abort();
    }

    PHYSFS_sint64 len = PHYSFS_fileLength(fp);
    if (len == -1) {
        show_error_msgbox("len was -1, wtf", ERROR_SOURCE_INTERNAL);
        abort();
    }

    char *text = xmalloc((size_t) len);

    PHYSFS_read(fp, text, sizeof(char), (PHYSFS_uint32) len);

    JSON_Value *root = json_parse_string(text);
    JSON_Object *rootobj = json_object(root);

    free(text);

    JSON_Array *anims = json_object_get_array(rootobj, "anims");
    
    size_t sz = json_array_get_count(anims);

    ent->anims = xmalloc(sz * sizeof(anim));
    ent->animc = sz;

    for (size_t i = 0; i < sz; ++i) {
        const char *anim_name = json_array_get_string(anims, i);
        char animpath[256];

        snprintf(animpath, 256, "%s/%s", path, anim_name);
        anim_load(&ent->anims[i], renderer, animpath, anim_name);
    } 

    json_value_free(root);

    ent->current_anim = NULL;
}

void entity_play_anim(entity *ent, const char *name)
{
    if (ent->current_anim != NULL && !ent->current_anim->over) {
        return;
    }

    for (size_t i = 0; i < ent->animc; ++i) {
        if (strcmp(name, ent->anims[i].name) == 0) {
            if (ent->current_anim != NULL) {
                vec2 rotdelta = sum(ent->current_anim->spr.rotcenter, mul(ent->anims[i].spr.rotcenter, -1));

                ent->pos = sum(ent->pos, rotdelta);
            }

            ent->current_anim = &ent->anims[i];

            if (ent->anims[i].framec > 0) {
                ent->anims[i].over = false;
            }
            ent->anims[i].projspawned = false;
            ent->anims[i].once = false;
            ent->anims[i].logicalframe = 0;
        }
    }
}

void entity_free(entity *ent)
{
    for (size_t i = 0; i < ent->animc; ++i) {
        anim_free(&ent->anims[i]);
    }

    free(ent->anims);
}
