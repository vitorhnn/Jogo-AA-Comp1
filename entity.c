// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#include <physfs.h>

#include "3rdparty/parson/parson.h"

#include "common.h"
#include "entity.h"

void entity_load(entity *ent, const char *path)
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

    PHYSFS_readBytes(fp, text, (PHYSFS_uint64) len);

    JSON_Value *root = json_parse_string(text);
    JSON_Object *rootobj = json_object(root);

    free(text);

    PHYSFS_close(fp);

    JSON_Array *anims = json_object_get_array(rootobj, "anims");
    
    size_t sz = json_array_get_count(anims);

    ent->anims = xmalloc(sz * sizeof(anim));
    ent->animc = sz;

    for (size_t i = 0; i < sz; ++i) {
        const char *anim_name = json_array_get_string(anims, i);
        char animpath[256];

        snprintf(animpath, 256, "%s/%s", path, anim_name);
        anim_load(&ent->anims[i], animpath, anim_name);
    } 

    json_value_free(root);

    ent->current_anim = NULL;

    ent->haslegs = true;
    
    anim_load(&ent->staticleg, "assets/characters/legs", "legs");
    anim_load(&ent->legs, "assets/characters/legs_walking", "legs_walk");
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

            return;
        }
    }
}

void entity_think(entity *ent)
{
    vec2    absrot = sum(ent->pos, ent->current_anim->spr.rotcenter),
            absorigin = sum(ent->pos, ent->current_anim->spr.projorigin);

    float   distance = pointdistance(absorigin, absrot),
            angle = pointangle(absrot, absorigin) + ent->lookat;

    vec2 rotorigin = {
        absrot.x - distance * cosf(angle),
        absrot.y - distance * sinf(angle)
    };
    ent->rotorigin = rotorigin;

    anim_think(ent->current_anim);
    anim_think(&ent->legs);

    ent->real_think(ent);
}

void entity_paint(entity *ent, rect camera, unsigned diff)
{
    vec2 campos = {ent->pos.x - camera.x, ent->pos.y - camera.y};
    if (!ent->dead && ent->haslegs) {
        if (norm(ent->mov) > 0) {
            vec2 fodase = mul(ent->legs.spr.rotcenter, -1);
            vec2 fodasemais = sum(ent->current_anim->spr.rotcenter, fodase);
            fodasemais = sum(fodasemais, campos);
            anim_paint(&ent->legs, fodasemais, atan2f(ent->mov.y, ent->mov.x) + (FPI/2));
        } else {
            vec2 fodase = mul(ent->staticleg.spr.rotcenter, -1);
            vec2 fodasemais = sum(ent->current_anim->spr.rotcenter, fodase);
            fodasemais = sum(fodasemais, campos);
            anim_paint(&ent->staticleg, fodasemais, ent->lookat);
        }
    }

    vec2 corrected = mul(ent->mov, diff);

    corrected = sum(corrected, campos);

    anim_paint(ent->current_anim, corrected, ent->lookat);
}

void entity_free(entity *ent)
{
    for (size_t i = 0; i < ent->animc; ++i) {
        anim_free(&ent->anims[i]);
    }

    anim_free(&ent->legs);
    anim_free(&ent->staticleg);


    free(ent->anims);

    ent->free(ent);
}
