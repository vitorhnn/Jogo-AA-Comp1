// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#include <physfs.h>

#include "3rdparty/parson/parson.h"

#include "common.h"
#include "stage.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

static void json_load(stage *stage, const char *path)
{
    PHYSFS_file *fp = PHYSFS_openRead(path);

    if (fp == NULL) {
        show_error_msgbox("failed to json_load stage", ERROR_SOURCE_PHYSFS);
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

    free(text);

    JSON_Object *obj = json_object(root);
    JSON_Object *maincol = json_object_get_object(obj, "maincol");
    JSON_Array *cols = json_object_get_array(obj, "cols");
    
    rect col = {
        .x = (float) json_object_get_number(maincol, "x"),
        .y = (float) json_object_get_number(maincol, "y"),
        .w = (float) json_object_get_number(maincol, "w"),
        .h = (float) json_object_get_number(maincol, "h")
    };

    stage->maincol = col;

    stage->colc = json_array_get_count(cols);
    stage->colarray = xmalloc(stage->colc * sizeof(rect));

    for (size_t i = 0; i < stage->colc; ++i) {
        JSON_Object *colobj = json_array_get_object(cols, i);
        rect col = {
            .x = (float) json_object_get_number(colobj, "x"),
            .y = (float) json_object_get_number(colobj, "y"),
            .w = (float) json_object_get_number(colobj, "w"),
            .h = (float) json_object_get_number(colobj, "h")
        };

        memcpy(&stage->colarray[i], &col, sizeof(col));
    }
}

void stage_load(stage *stage, SDL_Renderer *renderer, const char *path)
{
    char jsonpath[64];

    snprintf(jsonpath, 64, "%s.json", path);

    json_load(stage, jsonpath);

    char pngpath[64];
    
    snprintf(pngpath, 64, "%s.png", path);

    sprite_load(&stage->background, renderer, pngpath);

    memset(stage->entwrapper, 0, sizeof(stage->entwrapper));
    memset(stage->projectiles, 0, sizeof(stage->projectiles));
    memset(stage->pickups, 0, sizeof(stage->pickups));
}

void stage_add_entity(stage *stage, entity *ent)
{
    for (size_t i = 0; i < ARRAY_SIZE(stage->entwrapper); ++i) {
        if (!stage->entwrapper[i].active) {
            stage->entwrapper[i].ent = ent;
            stage->entwrapper[i].active = true;
            return;
        }
    }
}

void stage_remove_entity(stage *stage, entity *ent)
{
    for (size_t i = 0; i < ARRAY_SIZE(stage->entwrapper); ++i) {
        if (stage->entwrapper[i].ent == ent) {
            stage->entwrapper[i].active = false;
            entity_free(stage->entwrapper[i].ent);
        }
    }
}

void stage_add_pickup(stage *stage, sprite *spr, vec2 pos, void(*callback)(void))
{
    for (size_t i = 0; i < ARRAY_SIZE(stage->pickups); ++i) {
        if (!stage->pickups[i].active) {
            pickup new = {
                .spr = spr,
                .pos = pos,
                .frames = 0,
                .active = true,
                .picked_callback = callback
            };

            stage->pickups[i] = new;
            return;
        }
    }
}

void stage_add_projectile(stage *stage, entity *shooter, sprite *spr, vec2 target, float speed, float damage)
{
    stage_add_projectile_ex(stage, shooter, spr, target, speed, damage, 0, false);
}

void stage_add_projectile_ex(stage *stage, entity *shooter, sprite *spr, vec2 target, float speed, float damage, float angle, bool piercing)
{
    for (size_t i = 0; i < ARRAY_SIZE(stage->projectiles); i++) {
        if (!stage->projectiles[i].active) {
            vec2 mov = get_vec(shooter->rotorigin, target);

            mov = rot_vec(mov, angle);

            projectile newp = {
                .spr = spr,
                .pos = shooter->rotorigin,
                .mov = mul(mov, speed),
                .angle = pointangle(shooter->rotorigin, target) - (FPI/2),
                .active = true,
                .enemy  = shooter->enemy,
                .damage = damage,
                .piercing = piercing
            };

            stage->projectiles[i] = newp;
            return;
        } 
    }
}

bool stage_is_anything_alive(stage *stage)
{
    for (size_t i = 0; i < ARRAY_SIZE(stage->entwrapper); ++i) {
        if (stage->entwrapper[i].active && 
            !stage->entwrapper[i].ent->dead &&
            stage->entwrapper[i].ent->enemy)
        {
            return true;
        }
    }

    return false;
}

static bool stage_is_anything_colliding(stage *stage, rect col)
{
    if (col.y <= stage->maincol.y || 
        col.y + col.h >= stage->maincol.y + stage->maincol.h || 
        col.x <= stage->maincol.x ||
        col.x + col.w >= stage->maincol.x + stage->maincol.w)
    {
        return true;
    }

    for (size_t i = 0; i < stage->colc; ++i) {
        if (fullcollide(col, stage->colarray[i])) {
            return true;
        }
    }

    return false;

}

static entity *projectile_entcollide(projectile *this, stage *stage)
{
    for (size_t i = 0; i < ARRAY_SIZE(stage->entwrapper); ++i) {
        if (stage->entwrapper[i].active) {
            entity *ent = stage->entwrapper[i].ent;
            rect col = {this->pos.x, this->pos.y, this->spr->w, this->spr->h};

            rect entcol = {
                .x = ent->pos.x,
                .y = ent->pos.y,
                .w = ent->current_anim->frames[ent->current_anim->curframe].w,
                .h = ent->current_anim->frames[ent->current_anim->curframe].h
            };

            if (fullcollide(col, entcol)) {
                return ent;
            }
        }
    }

    return NULL;
}

static void projectile_update(projectile *this, stage *stage)
{
    this->pos = sum(this->pos, this->mov);

    rect col = {
        this->pos.x,
        this->pos.y,
        this->spr->w,
        this->spr->h
    };

    entity *maybecol = projectile_entcollide(this, stage);

    if (maybecol && maybecol->enemy != this->enemy && !maybecol->dead) {
        maybecol->health -= this->damage;

        if (!this->piercing) {
            this->active = false;
        }
    }

    if (stage_is_anything_colliding(stage, col)) {
        this->active = false;
    }
}

bool stage_is_ent_colliding(stage *stage, entity *ent)
{
    rect col = {
        .x = ent->pos.x,
        .y = ent->pos.y,
        .w = ent->current_anim->frames[ent->current_anim->curframe].w,
        .h = ent->current_anim->frames[ent->current_anim->curframe].h
    };
    
    return stage_is_anything_colliding(stage, col);
}

static void pickup_think(pickup *this, stage *stage)
{
    this->frames++;

    if (this->frames > 1200) {
        this->active = false;
    }

    rect thiscol = {this->pos.x, this->pos.y, this->spr->w, this->spr->h};
    for (size_t i = 0; i < ARRAY_SIZE(stage->entwrapper); ++i) {
        if (stage->entwrapper[i].active && !stage->entwrapper[i].ent->enemy) {
            rect entcol = {
                stage->entwrapper[i].ent->pos.x,
                stage->entwrapper[i].ent->pos.y,
                stage->entwrapper[i].ent->current_anim->frames[stage->entwrapper[i].ent->current_anim->curframe].w,
                stage->entwrapper[i].ent->current_anim->frames[stage->entwrapper[i].ent->current_anim->curframe].h
            };

            if (fullcollide(thiscol, entcol)) {
                this->picked_callback();
                this->active = false;
            }
        }
    }
}

void stage_think(stage *stage)
{
    for (size_t i = 0; i < ARRAY_SIZE(stage->entwrapper); ++i) {
        if (stage->entwrapper[i].active) {
            entity_think(stage->entwrapper[i].ent);
        }
    }

    for (size_t i = 0; i < ARRAY_SIZE(stage->projectiles); ++i) {
        if (stage->projectiles[i].active) {
            projectile_update(&stage->projectiles[i], stage);
        }
    }

    for (size_t i = 0; i < ARRAY_SIZE(stage->pickups); ++i) {
        if (stage->pickups[i].active) {
            pickup_think(&stage->pickups[i], stage);
        }
    }
}

static void projectile_paint(projectile *this, SDL_Renderer *renderer, unsigned diff)
{
    vec2 corrected = mul(this->mov, diff);
    corrected = sum(corrected, this->pos);

    sprite_paint_less_ex(this->spr, renderer, corrected, this->angle);
}

void stage_paint(stage *stage, SDL_Renderer *renderer, unsigned diff)
{
    vec2 nullvec = {0, 0};
    sprite_paint(&stage->background, renderer, nullvec);

    for (size_t i = 0; i < ARRAY_SIZE(stage->entwrapper); ++i) {
        if (stage->entwrapper[i].active) {
            entity_paint(stage->entwrapper[i].ent, renderer, diff);
        }
    }

    for (size_t i = 0; i < ARRAY_SIZE(stage->projectiles); ++i) {
        if (stage->projectiles[i].active) {
            projectile_paint(&stage->projectiles[i], renderer, diff);
        }
    }

    for (size_t i = 0; i < ARRAY_SIZE(stage->pickups); ++i) {
        if (stage->pickups[i].active) {
            sprite_paint(stage->pickups[i].spr, renderer, stage->pickups[i].pos);
        }
    }
}

void stage_free(stage *stage)
{
    for (size_t i = 0; i < ARRAY_SIZE(stage->entwrapper); ++i) {
        if (stage->entwrapper[i].active) {
            entity_free(stage->entwrapper[i].ent);
        }
    }

    free(stage->colarray);
}
