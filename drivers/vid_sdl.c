// Copyright © 2020 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

// sdl driver: just werks

#include "../vid.h"
#include "../common.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

static SDL_Window *window;
static SDL_Renderer *renderer;

void vid_init(void)
{
    bool fullscreen = setting_boolvalue("r_fullscreen");
    int width  = setting_intvalue("r_width"),
        height = setting_intvalue("r_height");

    printf("vid_init: fullscreen %s, %dx%d\n", fullscreen ? "true" : "false", width, height);

    SDL_WindowFlags wflags = SDL_WINDOW_RESIZABLE;

    if (fullscreen) {
        wflags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }

    window = SDL_CreateWindow("joguin",
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               width,
                               height,
                               wflags);

    if (window == NULL) {
        show_error_msgbox("Failed to SDL_CreateWindow", ERROR_SOURCE_SDL);
        return false;
    }

    SDL_RendererFlags rflags = 0;

    if (setting_boolvalue("r_accelerated")) {
        rflags |= SDL_RENDERER_ACCELERATED;
    } else {
        rflags |= SDL_RENDERER_SOFTWARE;
    }

    renderer = SDL_CreateRenderer(window, -1, rflags);

    if (renderer == NULL) {
        SDL_DestroyWindow(window);
        show_error_msgbox("Failed to SDL_CreateRenderer", ERROR_SOURCE_SDL);
        return false;
    }

    SDL_RendererInfo rinfo;
    SDL_GetRendererInfo(renderer, &rinfo);

    printf("vid_init: using %s renderer\n", rinfo.name);


    SDL_DisableScreenSaver();
    return true;
}

void vid_quit(void)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void vid_set_draw_color(const vid_color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void vid_set_logical_size(int width, int height)
{
    SDL_RenderSetLogicalSize(renderer, width, height);
}

vid_texture *vid_load_texture(SDL_RWops *rw)
{
    vid_texture *out = xmalloc(sizeof(vid_texture));

    SDL_Surface *surf = IMG_Load_RW(rw, 0);

    if (surf == NULL) {
        goto fail;
    }

    out->width = surf->w;
    out->height = surf->h;

    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);

    if (tex == NULL) {
        goto fail;
    }

    out->driver_data = tex;

    return out;

fail:
    free(out);
    return NULL;
}

void vid_free_texture(vid_texture *texture)
{
    SDL_Texture *sdl_tex = texture->driver_data;
    SDL_DestroyTexture(sdl_tex);
    free(texture);
}

void vid_push_draw_cmd(vid_draw_cmd *cmd)
{
    SDL_Rect src = {
        .x = (int) cmd->src.x,
        .y = (int) cmd->src.y,
        .w = (int) cmd->src.w,
        .h = (int) cmd->src.h
    };

    SDL_Rect dst = {
        .x = (int) cmd->dst.x,
        .y = (int) cmd->dst.y,
        .w = (int) cmd->dst.w,
        .h = (int) cmd->dst.h
    };

    SDL_Point center = {
        .x = (int) cmd->center.x,
        .y = (int) cmd->center.y
    };

    // SDL uses degrees for whatever god forsaken reason
    double degrees = (cmd->angle * (180 / acos(-1)));
    SDL_RenderCopyEx(renderer, cmd->texture->driver_data, &src, &dst, degrees, &center, SDL_FLIP_NONE);
}

void vid_clear(void)
{
    SDL_RenderClear(renderer);
}

void vid_present(void)
{
    SDL_RenderPresent(renderer);
}
