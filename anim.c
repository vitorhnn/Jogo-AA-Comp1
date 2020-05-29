// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#include <physfs.h>

#include "3rdparty/parson/parson.h"

#include "common.h"
#include "anim.h"

static void json_parse(anim *anim, const char *path)
{
    PHYSFS_file *fp = PHYSFS_openRead(path);

    if (fp == NULL) {
        show_error("shit failed", ERROR_SOURCE_PHYSFS);
        return;
    }
    
    PHYSFS_sint64 len = PHYSFS_fileLength(fp);
    if (len == -1) {
        show_error_msgbox("len was -1, wtf", ERROR_SOURCE_INTERNAL);
        abort();
    }

    char *text = xmalloc((size_t) len);

    memset(text, 0, (size_t) len);

    PHYSFS_readBytes(fp, text, (PHYSFS_uint64) len);

    JSON_Value *root = json_parse_string(text);

    free(text);

    PHYSFS_close(fp);

    JSON_Object *obj = json_object(root);
    JSON_Object *frames = json_object_get_object(obj, "frames");
    JSON_Object *meta = json_object_get_object(obj, "meta");
    
    size_t emptyframes = (size_t) (json_object_get_number(meta, "emptyframes"));

    if (frames != NULL) {
        anim->framec = json_object_get_count(frames);
        anim->frames = xmalloc((anim->framec + emptyframes) * sizeof(rect));

        for (size_t i = 0; i < anim->framec; ++i) {
            JSON_Object *frame = json_object(json_object_get_value_at(frames, i)); 

            anim->frames[i].x = (float) (json_object_dotget_number(frame, "frame.x"));
            anim->frames[i].y = (float) (json_object_dotget_number(frame, "frame.y"));
            anim->frames[i].w = (float) (json_object_dotget_number(frame, "frame.w"));
            anim->frames[i].h = (float) (json_object_dotget_number(frame, "frame.h"));
        }
        anim->over = false;
    } else {
        anim->framec = 0;
        anim->over = true;
        anim->frames = xmalloc((emptyframes + 1) * sizeof(rect));

        anim->frames[0].x = 0;
        anim->frames[0].y = 0;
        anim->frames[0].w = anim->spr.w;
        anim->frames[0].h = anim->spr.h;
    }


    anim->spr.projorigin.x = (float) (json_object_dotget_number(meta, "origin.x"));
    anim->spr.projorigin.y = (float) (json_object_dotget_number(meta, "origin.y"));

    anim->spr.rotcenter.x = (float) (json_object_dotget_number(meta, "rot.x"));
    anim->spr.rotcenter.y = (float) (json_object_dotget_number(meta, "rot.y"));

    anim->origframe = (size_t) (json_object_get_number(meta, "originframe"));


    for (size_t i = 0; i < emptyframes; ++i) {
        memcpy(&anim->frames[anim->framec + i], &anim->frames[0], sizeof(rect)); 
    }

    anim->framec += emptyframes;

    anim->once = false;

    json_value_free(root);
}

void anim_load(anim *anim, const char *path, const char *name)
{
    char pngpath[256];
    snprintf(pngpath, 256, "%s.png", path);

    sprite_load(&anim->spr, pngpath);

    char jsonpath[256];
    snprintf(jsonpath, 256, "%s.json", path);

    json_parse(anim, jsonpath);

    anim->curframe = 0;

    memset(anim->name, 0, sizeof(anim->name));
    strncpy(anim->name, name, sizeof(anim->name));
}

void anim_think(anim *anim)
{
    if (anim->framec != 0) {
        anim->logicalframe++;

        anim->curframe = anim->logicalframe / 4;
        if (anim->curframe >= anim->framec) {
            anim->logicalframe = 0;
            anim->curframe = 0;
            anim->over = true;
        }

        if (anim->curframe == anim->origframe && !anim->once) {
            anim->once = true;
            anim->projspawned = true;
        }
    }
}

void anim_paint(anim *anim, vec2 pos, float angle)
{
    sprite_paint_ex(&anim->spr, anim->frames[anim->curframe], pos, angle);
}

void anim_free(anim *anim)
{
    free(anim->frames);
    sprite_free(&anim->spr);
}
