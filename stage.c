// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#include <physfs.h>

#include "common.h"
#include "stage.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

static float parse_line(char *line)
{
    for (; *line != ':'; ++line); ++line;

    return strtof(line, NULL);
}

static void ugly_old_hack(stage *stage, const char *path)
{
    PHYSFS_file *fp = PHYSFS_openRead(path); 

    if (fp == NULL) {
        show_error("wwwwwwwwwwwooooooooords", ERROR_SOURCE_PHYSFS);
        return;
    }

    Sint64 len = PHYSFS_fileLength(fp);

    if (len == -1) {
        show_error("len was -1", ERROR_SOURCE_INTERNAL);
        return;
    }

    char *text = xmalloc((size_t) len + 1);

    memset(text, 0, (size_t) (len) + 1);

    PHYSFS_read(fp, text, sizeof(char), (unsigned) len);

    char *line = strtok(text, "\n");

    if (line == NULL) return;

    if (strncmp("BACKGROUND", line, 10) != 0) {
        return;
    }

    unsigned colidx = 0;
    bool is_main_col = true;

    for (; line; line = strtok(NULL, "\n")) {
        if (strncmp("cols", line, 4) == 0) {
            is_main_col = false;

            size_t arrsz = (size_t) (parse_line(line));

            stage->colc = arrsz;
            stage->colarray = xmalloc(arrsz * sizeof(rect));
        } else if (strncmp("col", line, 3) == 0) {
            colidx++;
        } else {
            switch (*line) {
                case 'x':
                    if (is_main_col) {
                        stage->maincol.x = parse_line(line);
                    } else {
                        stage->colarray[colidx - 1].x = parse_line(line);
                    }
                    break;
                case 'y':
                    if (is_main_col) {
                        stage->maincol.y = parse_line(line);
                    } else {
                        stage->colarray[colidx - 1].y = parse_line(line);
                    }
                    break;
                case 'w':
                    if (is_main_col) {
                        stage->maincol.w = parse_line(line);
                    } else {
                        stage->colarray[colidx - 1].w = parse_line(line);
                    }
                    break;
                case 'h':
                    if (is_main_col) {
                        stage->maincol.h = parse_line(line);
                    } else {
                        stage->colarray[colidx - 1].h = parse_line(line);
                    }
                    break;
            }
        }
    }

    free(text);
}

void stage_load(stage *stage, SDL_Renderer *renderer, const char *path)
{
    char mnftpath[64];

    snprintf(mnftpath, 64, "%s.manifest", path);

    ugly_old_hack(stage, mnftpath);

    char pngpath[64];
    
    snprintf(pngpath, 64, "%s.png", path);

    sprite_load(&stage->background, renderer, pngpath);

    memset(stage->entwrapper, 0, sizeof(stage->entwrapper));
    memset(stage->projectiles, 0, sizeof(stage->projectiles));
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
            stage->entwrapper[i].ent->free(stage->entwrapper[i].ent);
        }
    }
}

void stage_add_projectile(stage *stage, entity *shooter, sprite *spr, vec2 target, float speed)
{
    stage_add_projectile_ex(stage, shooter, spr, target, speed, 0);
}

void stage_add_projectile_ex(stage *stage, entity *shooter, sprite *spr, vec2 target, float speed, float angle)
{
    vec2 mov = get_vec(shooter->rotorigin, target);

    mov = rot_vec(mov, angle);

    projectile newp = {
        .spr = spr,
        .pos = shooter->rotorigin,
        .mov = mul(mov, speed),
        .angle = pointangle(shooter->rotorigin, target) - (FPI/2),
        .active = true,
        .enemy  = shooter->enemy
    };

    for (size_t i = 0; i < ARRAY_SIZE(stage->projectiles); i++) {
        if (!stage->projectiles[i].active) {
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
        maybecol->health -= 5;
        this->active = false;
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
}

void stage_free(stage *stage)
{
    for (size_t i = 0; i < ARRAY_SIZE(stage->entwrapper); ++i) {
   
    }

    free(stage->colarray);
}
