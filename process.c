#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <duktape.h>
#include "config.h"

duk_ret_t duk_process_quit(duk_context* ctx)
{
  duk_int_t ret = duk_get_int_default(ctx, 0, 0);
  SDL_Quit();
  exit(ret);
  return 0;
}

void duk_process_init(duk_context* ctx, int argc, char** argv)
{
  duk_idx_t proc_idx = duk_push_object(ctx);
  //duk_push_int(ctx, argc);
  //duk_put_prop_string(ctx, proc_idx, "argc");
  duk_idx_t argv_idx = duk_push_array(ctx);
  for(int i=0; i<argc; i++)
  {
    duk_push_string(ctx, argv[i]);
    duk_put_prop_index(ctx, argv_idx, i);
  }
  duk_put_prop_string(ctx, proc_idx, "argv");
  duk_push_c_function(ctx, duk_process_quit, 1);
  duk_put_prop_string(ctx, proc_idx, "quit");
  duk_push_string(ctx, TIEWRAP_VERSION);
  duk_put_prop_string(ctx, proc_idx, "version");
  duk_put_global_string(ctx, "process");
}