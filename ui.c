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
    vec2i   mousepos;
    bool    mousedown;
} ui_state;

typedef enum {
    BUTTON
} ui_type;

typedef struct {
    SDL_Texture *tex;
    char        *text;
    bool        valid;
    vec2i       pos;
    int         w, h;
} ui_button_t;

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

static bool mouse_in_rect(vec2i pos, int w, int h) {
    if (state.mousepos.x < pos.x || state.mousepos.y < pos.y) {
        return false;
    }
    if (state.mousepos.x > pos.x + w || state.mousepos.y > pos.y + h) {
        return false;
    }

    return true;
}

void ui_init(void) {
    state.mousepos.x = 0;
    state.mousepos.y = 0;
    state.mousedown = false;

    vector_init(&elements, 20);

    SDL_RWops *ops = PHYSFSRWOPS_openRead("opensans.ttf");
    ui_font = TTF_OpenFontRW(ops, 1, 40);
}

void ui_handle(SDL_Event *event) {
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
}

void ui_paint(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);

    SDL_Rect r = { state.mousepos.x, state.mousepos.y, 200, 200 };
    SDL_RenderFillRect(renderer, &r);

    for (size_t i = 0; i < elements.used; i++) {
        ui_element *el = (ui_element *) elements.data[i];

        switch (el->type) {
            case BUTTON: {
                ui_button_t *btn = (ui_button_t *) el->data;

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
        }
    }
}

void ui_quit(void) {
    for (size_t i = 0; i < elements.used; i++) {
        ui_element *el = (ui_element *) elements.data[i];
        
        free(el->data);
        free(el);
    }

    vector_free(&elements);

    TTF_CloseFont(ui_font);
}

static bool button_render_text(SDL_Renderer *renderer, ui_button_t *btn) {
    SDL_Color c = {0, 255, 0, SDL_ALPHA_OPAQUE};

    SDL_Surface *surf = TTF_RenderUTF8_Blended(ui_font, btn->text, c);
    
        if (surf == NULL) {
        show_error("button_render_text: TTF_RenderUTF8_Blended failed", ERROR_SOURCE_SDL);
        goto surf_fail;
    }
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
    if (tex == NULL) {
        show_error("button_render_text: SDL_CreateTextureFromSurface failed", ERROR_SOURCE_SDL);
        goto tex_fail;
    }

    btn->w = surf->w;
    btn->h = surf->h;
    btn->tex = tex;

    SDL_FreeSurface(surf);
    return true;

tex_fail:
    SDL_DestroyTexture(tex);
surf_fail:
    SDL_FreeSurface(surf);
    return false;
}

bool ui_button(int id, const char *text, vec2i pos) {
    for (size_t i = 0; i < elements.used; i++) {
        ui_element *element = (ui_element*) elements.data[i];
        if (element->id == id) {
            element->should_draw = true;
            
            // assert element->type == BUTTON

            ui_button_t *btn = (ui_button_t*) element->data;

            if (mouse_in_rect(btn->pos, btn->w, btn->h) && state.mousedown) {
                return true;
            }

            return false;
        }
    }
    
    ui_button_t *btn = xmalloc(sizeof(ui_button_t));

    btn->pos    = pos;
    btn->text   = strdup(text);
    btn->valid  = false;
   
    ui_element *el = xmalloc(sizeof(ui_element));

    el->id = id;
    el->type = BUTTON;
    el->should_draw = true;
    el->data = btn;

    // we can't actually know if the user clicked right now, as w and h are invalid
    // we need to wait for a render frame, so just say that the user didn't click.

    vector_insert(&elements, el);
 
    return false;
}
