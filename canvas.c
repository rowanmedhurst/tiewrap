#include <stdio.h>
#include <SDL2/SDL.h>
#include <duktape.h>
#include "config.h"

SDL_Window* cwindow = NULL;

static duk_ret_t duk_canvas_getcontext(duk_context* ctx)
{
  const char* type = duk_require_string(ctx, 0);
  duk_push_object(ctx);
  //TODO: return a different object whether we have type == '2d' or 'gl' or 'webgl' or whatever
  return 1;
}

static duk_ret_t duk_canvas_height_get(duk_context* ctx)
{
  int h;
  SDL_GetWindowSize(cwindow, NULL, &h);
  duk_push_int(ctx, h);
  return 1;
}
static duk_ret_t duk_canvas_height_set(duk_context* ctx)
{
  const int h = duk_require_int(ctx, 0);
  int w;
  SDL_GetWindowSize(cwindow, &w, NULL);
  SDL_SetWindowSize(cwindow, w, h);
  return 0;
}

static duk_ret_t duk_canvas_width_get(duk_context* ctx)
{
  int w;
  SDL_GetWindowSize(cwindow, &w, NULL);
  duk_push_int(ctx, w);
  return 1;
}
static duk_ret_t duk_canvas_width_set(duk_context* ctx)
{
  const int w = duk_require_int(ctx, 0);
  int h;
  SDL_GetWindowSize(cwindow, NULL, &h);
  SDL_SetWindowSize(cwindow, w, h);
  return 0;
}

void duk_canvas_init(duk_context* ctx, SDL_Window* win)
{
  cwindow = win;

  duk_idx_t canvas_idx = duk_push_object(ctx);

  duk_push_c_function(ctx, duk_canvas_getcontext, 1);
  duk_put_prop_string(ctx, canvas_idx, "getContext");

  duk_push_string(ctx, "height");
  duk_push_c_function(ctx, duk_canvas_height_get, 0);
  duk_push_c_function(ctx, duk_canvas_height_set, 1);
  duk_def_prop(ctx, canvas_idx, DUK_DEFPROP_HAVE_GETTER|DUK_DEFPROP_HAVE_SETTER);

  duk_push_string(ctx, "width");
  duk_push_c_function(ctx, duk_canvas_width_get, 0);
  duk_push_c_function(ctx, duk_canvas_width_set, 1);
  duk_def_prop(ctx, canvas_idx, DUK_DEFPROP_HAVE_GETTER|DUK_DEFPROP_HAVE_SETTER);

  duk_put_global_string(ctx, "canvas");
}