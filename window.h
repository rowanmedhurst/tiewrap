#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>
#include <duktape.h>

void duk_window_init(duk_context* ctx, SDL_Window* win);

#endif