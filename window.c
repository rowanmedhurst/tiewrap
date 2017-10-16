#include <stdio.h>
#include <SDL2/SDL.h>
#include <duktape.h>
#include "config.h"

SDL_Window* window = NULL;

static duk_ret_t duk_window_title_get(duk_context* ctx)
{
  //duk_push_this(ctx);
  //duk_push_current_function(ctx);
  //const char* key = duk_require_string(ctx, -1);
  //printf("[duk_window_title_get] debug\n");
  const char* title = SDL_GetWindowTitle(window);
  duk_push_string(ctx, title);
  return 1;
}
static duk_ret_t duk_window_title_set(duk_context* ctx)
{
  const char* str = duk_require_string(ctx, 0);
  //printf("[duk_window_title_set] debug: %s\n", str);
  SDL_SetWindowTitle(window, str);
  return 0;
}

void duk_window_init(duk_context* ctx, SDL_Window* win)
{
  window = win;
  duk_idx_t win_idx = duk_push_object(ctx);
  duk_push_boolean(ctx, 0);
  duk_put_prop_string(ctx, win_idx, "fullScreen");
  duk_push_uint(ctx, DEFAULT_HEIGHT);
  duk_put_prop_string(ctx, win_idx, "innerHeight");
  duk_push_uint(ctx, DEFAULT_WIDTH);
  duk_put_prop_string(ctx, win_idx, "innerWidth");
  duk_push_string(ctx, "title");
  duk_push_c_function(ctx, duk_window_title_get, 0);
  duk_push_c_function(ctx, duk_window_title_set, 1);
  duk_def_prop(ctx, win_idx, DUK_DEFPROP_HAVE_GETTER|DUK_DEFPROP_HAVE_SETTER);
  duk_put_global_string(ctx, "window");
}
