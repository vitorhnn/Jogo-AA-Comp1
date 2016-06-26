// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#include <physfs.h>

#include "3rdparty/parson/parson.h"

#include "common.h"
#include "anim.h"

static void json_parse(anim *anim, const char *path)
{
    PHYSFS_file *fp = PHYSFS_openRead(path);
    
    PHYSFS_sint64 len = PHYSFS_fileLength(fp);
    if (len == -1) {
        show_error_msgbox("len was -1, wtf", ERROR_SOURCE_INTERNAL);
        abort();
    }

    char *text = xmalloc((size_t) len);

    PHYSFS_read(fp, text, sizeof(char), (PHYSFS_uint32) len);

    JSON_Value *root = json_parse_string(text);

    free(text);

    JSON_Object *obj = json_object(root);
    JSON_Object *frames = json_object_get_object(obj, "frames");
    JSON_Object *meta = json_object_get_object(obj, "meta");

    if (frames != NULL) {
        anim->framec = json_object_get_count(frames);
        anim->frames = xmalloc(anim->framec * sizeof(rect));

        for (size_t i = 0; i < anim->framec; ++i) {
            JSON_Object *frame = json_object(json_object_get_value_at(frames, i)); 

            anim->frames[i].x = (float) (json_object_dotget_number(frame, "frame.x"));
            anim->frames[i].y = (float) (json_object_dotget_number(frame, "frame.y"));
            anim->frames[i].w = (float) (json_object_dotget_number(frame, "frame.w"));
            anim->frames[i].h = (float) (json_object_dotget_number(frame, "frame.h"));
        }
    } else {
        anim->framec = 0;
        anim->frames = xmalloc(sizeof(rect));

        anim->frames[0].x = 0;
        anim->frames[0].y = 0;
        anim->frames[0].w = anim->spr.w;
        anim->frames[0].h = anim->spr.h;
    }

    anim->spr.projorigin.x = (float) (json_object_dotget_number(meta, "origin.x"));
    anim->spr.projorigin.y = (float) (json_object_dotget_number(meta, "origin.y"));

    anim->spr.rotcenter.x = (float) (json_object_dotget_number(meta, "rot.x"));
    anim->spr.rotcenter.y = (float) (json_object_dotget_number(meta, "rot.y"));
}

void anim_load(anim *anim, SDL_Renderer *renderer, const char *path)
{
    char pngpath[256];
    snprintf(pngpath, 256, "%s.png", path);

    sprite_load(&anim->spr, renderer, pngpath);

    char jsonpath[256];
    snprintf(jsonpath, 256, "%s.json", path);

    json_parse(anim, jsonpath);

    anim->curframe = 0;
}

void anim_paint(anim *anim, SDL_Renderer* renderer, vec2 pos, float angle)
{
    sprite_paint_ex(&anim->spr, renderer, anim->frames[anim->curframe], pos, angle);

    anim->curframe++;

    if (anim->curframe == anim->framec) {
        anim->curframe = 0;
    }
}

void anim_free(anim *anim)
{
    free(anim->frames);
}
