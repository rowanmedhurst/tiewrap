#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <duktape.h>
#include <console/duk_console.h>
#include <module-node/duk_module_node.h>
#include "module.h"
#include "process.h"
#include "window.h"
#include "config.h"

void logSDLError(char* where, char* type)
{
  fprintf(stderr, "[%s] %s: %s\n", where, type, SDL_GetError());
  fflush(stderr);
}

static duk_ret_t get_error_stack(duk_context *ctx, __attribute__((unused)) void *udata)
{
  if (duk_is_error(ctx, 0) && duk_has_prop_string(ctx, 0, "stack"))
  {
    duk_get_prop_string(ctx, 0, "stack");
    duk_remove(ctx, -2);
  }
  return 1;
}

static void print_error(duk_context* ctx)
{
  duk_safe_call(ctx, get_error_stack, NULL, 1, 1);
  fprintf(stderr, "[duk] error: %s\n", duk_safe_to_string(ctx, -1));
  fflush(stderr);
  duk_pop(ctx);
}

static int handle_file(duk_context* ctx, char *path)
{
  if (push_file_contents(ctx, path))
  {
    print_error(ctx);
    return 1;
  }
  if (duk_module_node_peval_main(ctx, path))
  {
    print_error(ctx);
    return 1;
  }
  return 0;
}

void fatal_error_handler(__attribute__((unused)) void *udata, const char *msg)
{
  fputs("[duk] fatal", stderr);
  if (msg)
  {
    fputs(": ", stderr);
    fputs(msg, stderr);
  }
  fputc('\n', stderr);
  SDL_Quit();
  exit(1);
}

int main(int argc, char* argv[])
{
  // init SDL
  if (SDL_Init(SDL_INIT_VIDEO) != 0)
  {
    logSDLError("SDL_Init", "fatal");
    return 1;
  }
  // init window
  SDL_Window* win = SDL_CreateWindow("Tiewrap " TIEWRAP_VERSION, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, DEFAULT_WIDTH, DEFAULT_HEIGHT, SDL_WINDOW_SHOWN);
  if (win == NULL)
  {
    logSDLError("SDL_CreateWindow", "fatal");
    SDL_Quit();
    return 1;
  }
  //SDL_Surface* screen = SDL_GetWindowSurface(win);
  //SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF));
  //SDL_UpdateWindowSurface(win);

  // create javascript interpreter
  duk_context* ctx = duk_create_heap(NULL, NULL, NULL, NULL, fatal_error_handler);
  // put some useful stuff inside built-in modules
  duk_push_object(ctx);
  duk_push_c_function(ctx, cb_resolve_module, DUK_VARARGS);
  duk_put_prop_string(ctx, -2, "resolve");
  duk_push_c_function(ctx, cb_load_module, DUK_VARARGS);
  duk_put_prop_string(ctx, -2, "load");
  duk_module_node_init(ctx);
  duk_console_init(ctx, 0);
  duk_process_init(ctx, argc, argv);
  duk_window_init(ctx, win);

  // check if we have a script to run
  if(argc > 1)
  {
    handle_file(ctx, argv[1]);
  }
  else
  {
    duk_eval_string(ctx, "console.log('Tiewrap v" TIEWRAP_VERSION " - No script loaded.');");
    // Maybe go in a REPL here
  }

  // now we get into the SDL loop
  SDL_Event e;
  int quit = 0;

  while(quit == 0)
  {
    while(SDL_PollEvent(&e)!=0)
    {
      if(e.type == SDL_QUIT)
      {
        // onquit event here, return value is set to the quit variable so it can cancel it
        quit = 1;
      }
    }
  }

  // get exit code
  duk_push_global_object(ctx);
  duk_get_prop_string(ctx, -1, "process");
  duk_get_prop_string(ctx, -1, "exitCode");
  int exitCode = duk_get_int_default(ctx, -1, quit-1);

  // by now the program has ended
  duk_destroy_heap(ctx);
  SDL_DestroyWindow(win);
  SDL_Quit();
  return exitCode;
}
