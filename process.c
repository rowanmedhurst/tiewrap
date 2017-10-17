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

  const char* platform = SDL_GetPlatform();
  duk_push_string(ctx, platform);
  duk_put_prop_string(ctx, proc_idx, "platform");

  duk_push_c_function(ctx, duk_process_quit, 1);
  duk_put_prop_string(ctx, proc_idx, "quit");

  duk_push_string(ctx, TIEWRAP_VERSION);
  duk_put_prop_string(ctx, proc_idx, "version");

  duk_idx_t ver_idx = duk_push_array(ctx);
  
    duk_push_string(ctx, DUK_GIT_DESCRIBE);
    duk_put_prop_string(ctx, ver_idx, "duktape");

    char sdlverstr[10];
    SDL_version sdlver;
    SDL_VERSION(&sdlver);
    sprintf(sdlverstr, "v%d.%d.%d", sdlver.major, sdlver.minor, sdlver.patch);
    duk_push_string(ctx, sdlverstr);
    duk_put_prop_string(ctx, ver_idx, "sdl");

    duk_push_string(ctx, TIEWRAP_VERSION);
    duk_put_prop_string(ctx, ver_idx, "tiewrap");

  duk_put_prop_string(ctx, proc_idx, "versions");

  duk_put_global_string(ctx, "process");
}
