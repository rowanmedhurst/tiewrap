#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <duktape.h>
#include "config.h"

duk_ret_t duk_process_exit(duk_context* ctx)
{
  duk_push_global_object(ctx);
  duk_get_prop_string(ctx, -1, "process");
  duk_get_prop_string(ctx, -1, "exitCode");
  int exitCode = duk_get_int_default(ctx, -1, 0);
  duk_pop_n(ctx, 3);
  duk_int_t ret = duk_get_int_default(ctx, 0, exitCode);
  SDL_Quit();
  exit(ret);
  return 0;
}

duk_ret_t duk_process_sleep(duk_context* ctx)
{
  duk_uint_t time = duk_get_uint(ctx, 0);
  SDL_Delay(time);
  return 0;
}

static duk_ret_t duk_process_stacksize_get(duk_context* ctx)
{
  duk_idx_t top = duk_get_top(ctx);
  duk_push_number(ctx, top);
  return 1;
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

  duk_push_c_function(ctx, duk_process_exit, 1);
  duk_put_prop_string(ctx, proc_idx, "exit");

  duk_push_int(ctx, 0);
  duk_put_prop_string(ctx, proc_idx, "exitCode");

  duk_push_int(ctx, getpid());
  duk_put_prop_string(ctx, proc_idx, "pid");

  duk_push_int(ctx, getppid());
  duk_put_prop_string(ctx, proc_idx, "ppid");

  const char* platform = SDL_GetPlatform();
  duk_push_string(ctx, platform);
  duk_put_prop_string(ctx, proc_idx, "platform");

  duk_push_c_function(ctx, duk_process_sleep, 1);
  duk_put_prop_string(ctx, proc_idx, "sleep");

  duk_push_string(ctx, "stackSize");
  duk_push_c_function(ctx, duk_process_stacksize_get, 0);
  duk_def_prop(ctx, proc_idx, DUK_DEFPROP_HAVE_GETTER);

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
