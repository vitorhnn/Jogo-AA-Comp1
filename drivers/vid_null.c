// Copyright © 2016 Victor Hermann "vitorhnn" Chiletto
// Licensed under the MIT/Expat license.

// null driver: does absolutely nothing
// might be useful for building dedicated servers, if this game ever goes multiplayer

#include "../vid.h"
#include "../common.h"

#warning ("Compiling null video driver, this is probably not what you want.")


void vid_init(void) {
}

void vid_quit(void) {
}

void vid_set_window_title(const char *title) {
#pragma unused (title)
}

void vid_set_draw_color(const vid_color color) {
#pragma unused (color)
}

void vid_set_logical_size(int width, int height) {
#pragma unused (width, height)
}

void vid_clear(void) {
}

void vid_present(void) {
}
