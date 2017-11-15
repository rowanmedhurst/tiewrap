#ifndef CANVAS_H
#define CANVAS_H

#include <SDL2/SDL.h>
#include <duktape.h>

void duk_canvas_init(duk_context* ctx, SDL_Window* win);

#endif