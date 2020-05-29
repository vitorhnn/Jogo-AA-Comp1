// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#include <physfs.h>

#include "3rdparty/parson/parson.h"

#include "common.h"
#include "stage.h"
#include "effect.h"


#define EFFECT_ACTIVE(x) (x && x->active)

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

    PHYSFS_readBytes(fp, text, (PHYSFS_uint64) len);

    JSON_Value *root = json_parse_string(text);

    free(text);

    PHYSFS_close(fp);

    JSON_Object *obj = json_object(root);
    JSON_Object *maincol = json_object_get_object(obj, "maincol");
    JSON_Array *cols = json_object_get_array(obj, "cols");
    JSON_Array *spawns = json_object_get_array(obj, "spawns");
    
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

    stage->spawnc = json_array_get_count(spawns);
    stage->spawns = xmalloc(stage->spawnc * sizeof(vec2));

    for (size_t i = 0; i < stage->spawnc; ++i) {
        JSON_Object *vecobj = json_array_get_object(spawns, i);

        vec2 vec = {
            .x = (float) json_object_get_number(vecobj, "x"),
            .y = (float) json_object_get_number(vecobj, "y")
        };

        memcpy(&stage->spawns[i], &vec, sizeof(vec2));
    }

    stage->playerspawn.x = (float) json_object_dotget_number(obj, "character_spawn.x");
    stage->playerspawn.y = (float) json_object_dotget_number(obj, "character_spawn.y");

    stage->playerexit.x = (float) json_object_dotget_number(obj, "character_exit.x");
    stage->playerexit.y = (float) json_object_dotget_number(obj, "character_exit.y");
    stage->playerexit.w = (float) json_object_dotget_number(obj, "character_exit.w");
    stage->playerexit.h = (float) json_object_dotget_number(obj, "character_exit.h");

    json_value_free(root);
}

void stage_load(stage *stage, const char *path)
{
    char jsonpath[64];

    snprintf(jsonpath, 64, "%s.json", path);

    json_load(stage, jsonpath);

    char pngpath[64];
    
    snprintf(pngpath, 64, "%s.png", path);

    sprite_load(&stage->background, pngpath);

    memcpy(stage->name, path, sizeof(char) * strlen(path));

    memset(stage->entwrapper, 0, sizeof(stage->entwrapper));
    memset(stage->projectiles, 0, sizeof(stage->projectiles));
    memset(stage->pickups, 0, sizeof(stage->pickups));
    memset(stage->effects, 0, sizeof(stage->effects));
}

bool stage_is_spawn_visible(vec2 spawn, rect camera)
{
    rect fakecol = {spawn.x, spawn.y, 0, 0};

    if (fullcollide(fakecol, camera)) {
        return true;
    }
    
    return false;
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

void stage_add_effect(stage *stage, effect *fx)
{
    for (size_t i = 0; i < ARRAY_SIZE(stage->effects); ++i) {
        if (!EFFECT_ACTIVE(stage->effects[i])) {
            stage->effects[i] = fx;
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

            if (fullcollide(col, entcol) && ent->enemy != this->enemy && !ent->dead) {
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

    if (maybecol) {
        maybecol->health -= this->damage;

        effect *FUCK = effect_load(this->pos, this->angle - FPI, "assets/characters/blood");

        stage_add_effect(stage, FUCK);

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

    for (size_t i = 0; i < ARRAY_SIZE(stage->effects); ++i) {
        if (EFFECT_ACTIVE(stage->effects[i])) {
            effect_think(stage->effects[i]);

            if (!stage->effects[i]->active) {
                effect_free(stage->effects[i]);
                stage->effects[i] = NULL;
            }
        }
    }
}

static void projectile_paint(projectile *this, rect camera, unsigned diff)
{
    vec2 campos = {this->pos.x - camera.x, this->pos.y - camera.y};
    vec2 corrected = mul(this->mov, diff);
    corrected = sum(corrected, campos);

    sprite_paint_less_ex(this->spr, corrected, this->angle);
}

void stage_paint(stage *stage, rect camera, unsigned diff)
{
    sprite_paint_ex(&stage->background, camera, MAKEVEC(0, 0), 0);

    for (size_t i = 0; i < ARRAY_SIZE(stage->entwrapper); ++i) {
        if (stage->entwrapper[i].active) {
            entity_paint(stage->entwrapper[i].ent, camera, diff);
        }
    }

    for (size_t i = 0; i < ARRAY_SIZE(stage->projectiles); ++i) {
        if (stage->projectiles[i].active) {
            projectile_paint(&stage->projectiles[i], camera, diff);
        }
    }

    for (size_t i = 0; i < ARRAY_SIZE(stage->pickups); ++i) {
        if (stage->pickups[i].active) {
            vec2 pos = {stage->pickups[i].pos.x - camera.x, stage->pickups[i].pos.y - camera.y};
            sprite_paint(stage->pickups[i].spr, pos);
        }
    }

    for (size_t i = 0; i < ARRAY_SIZE(stage->effects); ++i) {
        if (EFFECT_ACTIVE(stage->effects[i])) {
            effect_paint(stage->effects[i], camera);
        }
    }
}

void stage_free(stage *stage)
{
    for (size_t i = 0; i < ARRAY_SIZE(stage->entwrapper); ++i) {
        if (stage->entwrapper[i].active && stage->entwrapper[i].ent->enemy) {
            stage->entwrapper[i].active = false;
            entity_free(stage->entwrapper[i].ent);
        }
    }

    free(stage->colarray);
    free(stage->spawns);

    sprite_free(&stage->background);
}
