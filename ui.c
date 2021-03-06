// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

#define _XOPEN_SOURCE 500

#include <stdbool.h>

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>

#include "3rdparty/physicsfs/extras/physfsrwops.h"

#include "containers/vector.h"

#include "common.h"
#include "ui.h"

typedef struct {
    int     activeitem;
    int     hotitem;
    vec2   mousepos;
    bool    mousedown;
} ui_state;

typedef enum {
    BUTTON,
    RECT
} ui_type;

typedef struct {
    SDL_Texture *tex;
    char        *text;
    bool        valid;
    vec2        pos;
    int         w, h;
    SDL_Color   color;
} ui_button_t;

typedef struct {
    rect actual;
    bool valid;
} ui_rect_t;

typedef struct {
    void    *data;
    int     id;
    bool    should_draw;
    ui_type type;
} ui_element;

static bool button_render_text(SDL_Renderer *, ui_button_t *);

static ui_state state;
static vector   elements;
static TTF_Font *ui_font;

static bool mouse_in_rect(vec2 pos, int w, int h)
{
    if (state.mousepos.x < pos.x || state.mousepos.y < pos.y) {
        return false;
    }

    if (state.mousepos.x > pos.x + w || state.mousepos.y > pos.y + h) {
        return false;
    }

    return true;
}

void ui_init(void)
{
    state.mousepos.x = 0;
    state.mousepos.y = 0;
    state.mousedown = false;

    vector_init(&elements, 20);

    SDL_RWops *ops = PHYSFSRWOPS_openRead("opensans.ttf");
    ui_font = TTF_OpenFontRW(ops, 1, 20);
}

void ui_handle(SDL_Event *event)
{
    state.hotitem = 0;

    switch (event->type) {
        case SDL_MOUSEMOTION:
            state.mousepos.x = event->motion.x;
            state.mousepos.y = event->motion.y;

            break;

        case SDL_MOUSEBUTTONDOWN:
            state.mousedown = true;
            break;

        case SDL_MOUSEBUTTONUP:
            state.mousedown = false;
            break;

        default:
            break;
    }

    for (size_t i = 0; i < elements.used; i++) {
        ui_element *el = elements.data[i];

        switch (el->type) {
            case BUTTON: {
                ui_button_t *btn = el->data;

                if (mouse_in_rect(btn->pos, btn->w, btn->h)) {
                    state.hotitem = el->id;

                    if (state.activeitem == 0 && state.mousedown) {
                        state.activeitem = el->id;
                    }
                }

                break;
            }
            case RECT: {
                ui_rect_t *rect = el->data;

                if (mouse_in_rect(MAKEVEC(rect->actual.x, rect->actual.y), rect->actual.w, rect->actual.h)) {
                    state.hotitem = el->id;

                    if (state.activeitem == 0 && state.mousedown) {
                        state.activeitem = el->id;
                    }
                }
            }
        }
    }
}

void ui_think(void)
{
    for (size_t i = 0; i < elements.used; i++) {
        ui_element *el = elements.data[i];
        el->should_draw = false;
    }
}

void ui_paint(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);

    for (size_t i = 0; i < elements.used; i++) {
        ui_element *el = elements.data[i];

        if (!el->should_draw) {
            continue;
        }

        switch (el->type) {
            case BUTTON: {
                ui_button_t *btn = el->data;

                if (!btn->valid) {
                    button_render_text(renderer, btn);
                }

                SDL_Rect rect = {
                    .x = btn->pos.x,
                    .y = btn->pos.y,
                    .w = btn->w,
                    .h = btn->h
                };

                SDL_RenderCopy(renderer, btn->tex, NULL, &rect);
            }
            break;
        }
    }

    if (!state.mousedown) {
        state.activeitem = 0;
    } else if (state.activeitem == 0) {
        state.activeitem = -1;
    }

}

void ui_quit(void)
{
    for (size_t i = 0; i < elements.used; i++) {
        ui_element *el = elements.data[i];

        switch (el->type) {
            case BUTTON: {
                ui_button_t *btn = el->data;

                free(btn->text);
                SDL_DestroyTexture(btn->tex);
                break;
            }
        }

        free(el->data);
        free(el);
    }

    vector_free(&elements);

    TTF_CloseFont(ui_font);
}

static bool button_render_text(SDL_Renderer *renderer, ui_button_t *btn)
{
    SDL_Surface *surf = TTF_RenderUTF8_Blended(ui_font, btn->text, btn->color);

    if (surf == NULL) {
        show_error("button_render_text: TTF_RenderUTF8_Blended failed", ERROR_SOURCE_SDL);
        return false;
    }

    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);

    if (tex == NULL) {
        show_error("button_render_text: SDL_CreateTextureFromSurface failed", ERROR_SOURCE_SDL);
        goto failure;
    }

    btn->w      = surf->w;
    btn->h      = surf->h;
    btn->tex    = tex;
    btn->valid  = true;

    SDL_FreeSurface(surf);
    return true;

failure:
    SDL_FreeSurface(surf);
    return false;
}

bool ui_rect(int id, rect rect)
{
    for (size_t i = 0; i < elements.used; i++) {
        ui_element *element = elements.data[i];

        if (element->id == id) {
            element->should_draw = true;

            if (!state.mousedown && state.hotitem == id && state.activeitem == id) {
                state.activeitem = 0;
                return true;
            }

            return false;
        }
    }

    ui_rect_t *r = xmalloc(sizeof(ui_button_t));

    r->actual = rect;
    r->valid = true;


    ui_element *el = xmalloc(sizeof(ui_element));

    el->id          = id;
    el->type        = RECT;
    el->should_draw = true;
    el->data        = r;

    // we can't actually know if the user clicked right now, as w and h are invalid
    // we need to wait for a render frame, so just say that the user didn't click.

    vector_insert(&elements, el);

    return false;
}

bool ui_button(int id, const char *text, vec2 pos, SDL_Color color)
{
    for (size_t i = 0; i < elements.used; i++) {
        ui_element *element = elements.data[i];

        if (element->id == id) {
            element->should_draw = true;

            ui_button_t *btn = element->data;

            if (strcmp(text, btn->text) != 0) {
                free(btn->text);
                btn->text = strdup(text);
                btn->valid = false;
            }

            if (btn->color.r != color.r ||
                    btn->color.g != color.g || // I'm not actually sure if just invalidating is faster
                    btn->color.b != color.b ||
                    btn->color.a != color.a) {
                btn->color = color;
                btn->valid = false;
            }

            if (!state.mousedown && state.hotitem == id && state.activeitem == id) {
                state.activeitem = 0;
                return true;
            }

            return false;
        }
    }

    ui_button_t *btn = xmalloc(sizeof(ui_button_t));

    btn->pos    = pos;
    btn->text   = strdup(text);
    btn->valid  = false;
    btn->color  = color;

    ui_element *el = xmalloc(sizeof(ui_element));

    el->id          = id;
    el->type        = BUTTON;
    el->should_draw = true;
    el->data        = btn;

    // we can't actually know if the user clicked right now, as w and h are invalid
    // we need to wait for a render frame, so just say that the user didn't click.

    vector_insert(&elements, el);

    return false;
}
