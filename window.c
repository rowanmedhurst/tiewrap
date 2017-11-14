#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <duktape.h>
#include "config.h"

SDL_Window* window = NULL;

static duk_ret_t duk_window_alert(duk_context* ctx)
{
  const char* msg = duk_get_string_default(ctx, 0, "");
  const char* title = SDL_GetWindowTitle(window);
  const char* ret = duk_get_string_default(ctx, 1, title);
  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, ret, msg, window);
  return 0;
}

static duk_ret_t duk_window_bordered_get(duk_context* ctx)
{
  const int flags = SDL_GetWindowFlags(window);
  duk_push_boolean(ctx, (flags&SDL_WINDOW_BORDERLESS)==0);
  return 1;
}
static duk_ret_t duk_window_bordered_set(duk_context* ctx)
{
  const int borders = duk_require_boolean(ctx, 0);
  SDL_SetWindowBordered(window, borders);
  return 0;
}

static duk_ret_t duk_window_brightness_get(duk_context* ctx)
{
  const float brightness = SDL_GetWindowBrightness(window);
  duk_push_number(ctx, brightness);
  return 1;
}
static duk_ret_t duk_window_brightness_set(duk_context* ctx)
{
  const double num = duk_require_number(ctx, 0);
  SDL_SetWindowBrightness(window, num);
  return 0;
}

static duk_ret_t duk_window_enablescreensaver_get(duk_context* ctx)
{
  duk_push_boolean(ctx, SDL_IsScreenSaverEnabled());
  return 1;
}
static duk_ret_t duk_window_enablescreensaver_set(duk_context* ctx)
{
  const int screensaver = duk_require_boolean(ctx, 0);
  if(screensaver)
  {
    SDL_EnableScreenSaver();
  }
  else
  {
    SDL_DisableScreenSaver();
  }
  return 0;
}

static duk_ret_t duk_window_innerheight_get(duk_context* ctx)
{
  int h;
  SDL_GetWindowSize(window, NULL, &h);
  duk_push_int(ctx, h);
  return 1;
}
static duk_ret_t duk_window_innerheight_set(duk_context* ctx)
{
  const int h = duk_require_int(ctx, 0);
  int w;
  SDL_GetWindowSize(window, &w, NULL);
  SDL_SetWindowSize(window, w, h);
  return 0;
}

static duk_ret_t duk_window_innerwidth_get(duk_context* ctx)
{
  int w;
  SDL_GetWindowSize(window, &w, NULL);
  duk_push_int(ctx, w);
  return 1;
}
static duk_ret_t duk_window_innerwidth_set(duk_context* ctx)
{
  const int w = duk_require_int(ctx, 0);
  int h;
  SDL_GetWindowSize(window, NULL, &h);
  SDL_SetWindowSize(window, w, h);
  return 0;
}

static duk_ret_t duk_window_on(duk_context* ctx)
{
  const char* event = duk_require_string(ctx, 0);
  duk_push_global_stash(ctx);
  //duk_idx_t event_idx = duk_get_prop_string(ctx, -1, "events");
  duk_dup(ctx, 1);
  duk_bool_t rc = duk_put_prop_string(ctx, -2, event);
  return 0;
}

static duk_ret_t duk_window_off(duk_context* ctx)
{
  const char* event = duk_require_string(ctx, 0);
  duk_push_global_stash(ctx);
  //duk_idx_t event_idx = duk_get_prop_string(ctx, -1, "events");
  duk_bool_t rc = duk_del_prop_string(ctx, -1, event);
  return 0;
}

#if SDL_VERSION_ATLEAST(2, 0, 5)
static duk_ret_t duk_window_opacity_get(duk_context* ctx)
{
  float opacity;
  SDL_GetWindowOpacity(window, &opacity);
  duk_push_number(ctx, opacity);
  return 1;
}
static duk_ret_t duk_window_opacity_set(duk_context* ctx)
{
  const double num = duk_require_number(ctx, 0);
  SDL_SetWindowOpacity(window, num);
  return 0;
}
#endif

static duk_ret_t duk_window_screenx_get(duk_context* ctx)
{
  int x;
  SDL_GetWindowPosition(window, &x, NULL);
  duk_push_int(ctx, x);
  return 1;
}
static duk_ret_t duk_window_screenx_set(duk_context* ctx)
{
  const int x = duk_require_int(ctx, 0);
  int y;
  SDL_GetWindowPosition(window, NULL, &y);
  SDL_SetWindowPosition(window, x, y);
  return 0;
}

static duk_ret_t duk_window_screeny_get(duk_context* ctx)
{
  int y;
  SDL_GetWindowPosition(window, NULL, &y);
  duk_push_int(ctx, y);
  return 1;
}
static duk_ret_t duk_window_screeny_set(duk_context* ctx)
{
  const int y = duk_require_int(ctx, 0);
  int x;
  SDL_GetWindowPosition(window, &x, NULL);
  SDL_SetWindowPosition(window, x, y);
  return 0;
}

static duk_ret_t duk_window_title_get(duk_context* ctx)
{
  const char* title = SDL_GetWindowTitle(window);
  duk_push_string(ctx, title);
  return 1;
}
static duk_ret_t duk_window_title_set(duk_context* ctx)
{
  const char* str = duk_require_string(ctx, 0);
  SDL_SetWindowTitle(window, str);
  return 0;
}

void duk_window_init(duk_context* ctx, SDL_Window* win)
{
  window = win;

  duk_idx_t win_idx = duk_push_object(ctx);

  duk_push_c_function(ctx, duk_window_on, 2);
  duk_put_prop_string(ctx, win_idx, "addEventListener");

  duk_push_c_function(ctx, duk_window_alert, 2);
  duk_put_prop_string(ctx, win_idx, "alert");

  duk_push_string(ctx, "bordered");
  duk_push_c_function(ctx, duk_window_bordered_get, 0);
  duk_push_c_function(ctx, duk_window_bordered_set, 1);
  duk_def_prop(ctx, win_idx, DUK_DEFPROP_HAVE_GETTER|DUK_DEFPROP_HAVE_SETTER);

  duk_push_string(ctx, "brightness");
  duk_push_c_function(ctx, duk_window_brightness_get, 0);
  duk_push_c_function(ctx, duk_window_brightness_set, 1);
  duk_def_prop(ctx, win_idx, DUK_DEFPROP_HAVE_GETTER|DUK_DEFPROP_HAVE_SETTER);
  
  duk_push_string(ctx, "enableScreenSaver");
  duk_push_c_function(ctx, duk_window_enablescreensaver_get, 0);
  duk_push_c_function(ctx, duk_window_enablescreensaver_set, 1);
  duk_def_prop(ctx, win_idx, DUK_DEFPROP_HAVE_GETTER|DUK_DEFPROP_HAVE_SETTER);

  //duk_push_string(ctx, "fullScreen");
  //duk_push_c_function(ctx, duk_window_fullscreen_get, 0);
  //duk_push_c_function(ctx, duk_window_fullscreen_set, 1);
  //duk_def_prop(ctx, win_idx, DUK_DEFPROP_HAVE_GETTER|DUK_DEFPROP_HAVE_SETTER);

  duk_push_string(ctx, "innerHeight");
  duk_push_c_function(ctx, duk_window_innerheight_get, 0);
  duk_push_c_function(ctx, duk_window_innerheight_set, 1);
  duk_def_prop(ctx, win_idx, DUK_DEFPROP_HAVE_GETTER|DUK_DEFPROP_HAVE_SETTER);

  duk_push_string(ctx, "innerWidth");
  duk_push_c_function(ctx, duk_window_innerwidth_get, 0);
  duk_push_c_function(ctx, duk_window_innerwidth_set, 1);
  duk_def_prop(ctx, win_idx, DUK_DEFPROP_HAVE_GETTER|DUK_DEFPROP_HAVE_SETTER);

  duk_push_c_function(ctx, duk_window_off, 1); 
  duk_put_prop_string(ctx, win_idx, "off");
  duk_push_c_function(ctx, duk_window_on, 2);
  duk_put_prop_string(ctx, win_idx, "on");

#if SDL_VERSION_ATLEAST(2, 0, 5)
  duk_push_string(ctx, "opacity");
  duk_push_c_function(ctx, duk_window_opacity_get, 0);
  duk_push_c_function(ctx, duk_window_opacity_set, 1);
  duk_def_prop(ctx, win_idx, DUK_DEFPROP_HAVE_GETTER|DUK_DEFPROP_HAVE_SETTER);
#endif

  duk_push_c_function(ctx, duk_window_off, 1);
  duk_put_prop_string(ctx, win_idx, "removeEventListener");

  duk_push_string(ctx, "screenX");
  duk_push_c_function(ctx, duk_window_screenx_get, 0);
  duk_push_c_function(ctx, duk_window_screenx_set, 1);
  duk_def_prop(ctx, win_idx, DUK_DEFPROP_HAVE_GETTER|DUK_DEFPROP_HAVE_SETTER);

  duk_push_string(ctx, "screenY");
  duk_push_c_function(ctx, duk_window_screeny_get, 0);
  duk_push_c_function(ctx, duk_window_screeny_set, 1);
  duk_def_prop(ctx, win_idx, DUK_DEFPROP_HAVE_GETTER|DUK_DEFPROP_HAVE_SETTER);

  char* subsystem = "Unknown";
  SDL_SysWMinfo info;
  if(SDL_GetWindowWMInfo(window,&info))
  {
    switch(info.subsystem)
    {
      case SDL_SYSWM_UNKNOWN:   break;
      case SDL_SYSWM_WINDOWS:   subsystem = "Windows";                break;
      case SDL_SYSWM_X11:       subsystem = "X11";                    break;
#if SDL_VERSION_ATLEAST(2, 0, 3)
      case SDL_SYSWM_WINRT:     subsystem = "WinRT";                  break;
#endif
      case SDL_SYSWM_DIRECTFB:  subsystem = "DirectFB";               break;
      case SDL_SYSWM_COCOA:     subsystem = "Cocoa";                  break;
      case SDL_SYSWM_UIKIT:     subsystem = "UIKit";                  break;
#if SDL_VERSION_ATLEAST(2, 0, 2)
      case SDL_SYSWM_WAYLAND:   subsystem = "Wayland";                break;
      case SDL_SYSWM_MIR:       subsystem = "Mir";                    break;
#endif
#if SDL_VERSION_ATLEAST(2, 0, 4)
      case SDL_SYSWM_ANDROID:   subsystem = "Android";                break;
#endif
#if SDL_VERSION_ATLEAST(2, 0, 5)
      case SDL_SYSWM_VIVANTE:   subsystem = "Vivante";                break;
#endif
#if SDL_VERSION_ATLEAST(2, 0, 6)
      case SDL_SYSWM_OS2:       subsystem = "OS/2";                   break;
#endif
    }
  }
  duk_push_string(ctx, subsystem);
  duk_put_prop_string(ctx, win_idx, "subsystem");

  duk_push_string(ctx, "title");
  duk_push_c_function(ctx, duk_window_title_get, 0);
  duk_push_c_function(ctx, duk_window_title_set, 1);
  duk_def_prop(ctx, win_idx, DUK_DEFPROP_HAVE_GETTER|DUK_DEFPROP_HAVE_SETTER);

  //duk_push_string(ctx, "visible");
  //duk_push_c_function(ctx, duk_window_visible_get, 0);
  //duk_push_c_function(ctx, duk_window_visible_set, 1);
  //duk_def_prop(ctx, win_idx, DUK_DEFPROP_HAVE_GETTER|DUK_DEFPROP_HAVE_SETTER);

  duk_put_global_string(ctx, "window");

  duk_push_c_function(ctx, duk_window_alert, 2);
  duk_put_global_string(ctx, "alert");

  duk_push_global_stash(ctx);
  duk_push_object(ctx);
  duk_put_prop_string(ctx, -2, "events");
  duk_pop(ctx);
}
