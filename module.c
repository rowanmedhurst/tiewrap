// Module search function
// Copyright (C) 2015 Legimet
//
// This file is part of Duktape-nspire.
//
// Duktape-nspire is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Duktape-nspire is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Duktape-nspire. If not, see <http://www.gnu.org/licenses/>.

#define _GNU_SOURCE
#include <stdbool.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <duktape.h>
#include "module.h"

// stole this from OpenBSD so I'm not linking it against libbsd just for this function
// so the code I stole from duktape-nspire works on Linux
size_t _strlcpy(char *dst, const char *src, size_t dsize)
{
  const char *osrc = src;
  size_t nleft = dsize;

  /* Copy as many bytes as will fit. */
  if (nleft != 0) {
    while (--nleft != 0) {
      if ((*dst++ = *src++) == '\0')
        break;
    }
  }

  /* Not enough room in dst, add NUL and traverse rest of src. */
  if (nleft == 0) {
    if (dsize != 0)
      *dst = '\0';    /* NUL-terminate dst */
    while (*src++)
      ;
  }

  return(src - osrc - 1); /* count does not include NUL */
}

duk_ret_t push_file_contents(duk_context *ctx, const char *filename) {
  FILE *file = fopen(filename, "rb");
  if (!file) goto error;
  if (fseek(file, 0, SEEK_END)) goto error;
  long size = ftell(file);
  if (size < 0) goto error;
  if (size == 0) {
    fclose(file);
    duk_push_lstring(ctx, NULL, 0);
    return 0;
  }
  if (fseek(file, 0, SEEK_SET)) goto error;
  char *buf = duk_push_fixed_buffer(ctx, size);
  if (fread(buf, size, 1, file) != 1) {
    duk_pop(ctx);
    goto error;
  }
  fclose(file);
  duk_buffer_to_string(ctx, -1);
  return 0;

error:
  if (file)
    fclose(file);
  return duk_error(ctx, DUK_ERR_TYPE_ERROR, "Unable to read '%s': %s", filename, strerror(errno));
}

duk_ret_t cb_resolve_module(duk_context *ctx) {
  /*
   *  Entry stack: [ requested_id parent_id ]
   */

  static const char * start[] = {"/", "./", "../"};
  static const char * ext[] = {".js", ".json",
    "/index.js", "/index.json"};

  const char *requested_id = duk_get_string(ctx, 0);
  const char *parent_id = duk_get_string(ctx, 1);  /* calling module */

  char resolved_id[PATH_MAX];

  // Check if it is a built-in C module
  for (unsigned i = 0; i < c_module_count; ++i) {
    if (!strcmp(c_module_list[i].name, requested_id)) {
      strcpy(resolved_id, requested_id);
      goto success;
    }
  }

  // External modules
  bool valid = false;
  for (unsigned i = 0; i < sizeof(start) / sizeof(start[0]); ++i) {
    if (!strncmp(requested_id, start[i], strlen(start[i]))) {
      valid = true;
      break;
    }
  }

  if (!valid) goto failure;

  char path[PATH_MAX];
  char *cur;
  struct stat stbuf;

  if (requested_id[0] == '/') { // Absolute path
    path[0] = '\0';
    cur = path;
  } else { // Relative path
    strcpy(path, parent_id);
    cur = strrchr(path, '/'); // Get rid of basename
    if (cur) ++cur;
    else cur = path;
  }

  size_t remaining = PATH_MAX - (cur - path);
  size_t copied;
  if ((copied = _strlcpy(cur, requested_id, remaining)) >= remaining) goto failure;
  remaining -= copied;
  cur += copied;

  // Check each of the extensions in order
  for (unsigned i = 0; i < sizeof(ext) / sizeof(ext[0]); ++i) {
    if (_strlcpy(cur, ext[i], remaining) >= remaining)
      goto failure;
    if (!stat(path, &stbuf) && S_ISREG(stbuf.st_mode)) {
      if (realpath(path, resolved_id))
        goto success;
    }
  }

failure:
  return duk_generic_error(ctx, "Could not find module '%s'", requested_id);

success:
  duk_push_string(ctx, resolved_id);
  return 1;  /*nrets*/

}

duk_ret_t cb_load_module(duk_context *ctx) {
  /*
   *  Entry stack: [ resolved_id exports module ]
   */

  const char *id = duk_get_string(ctx, 0); // Get the resolved id

  // C modules have ids that don't start with /
  if (id[0] != '/') {
    for (unsigned i = 0; i < c_module_count; i++) {
      if (!strcmp(c_module_list[i].name, id)) {
        // Call init function, enumerate properties of returned object,
        // and put them in exports
        c_module_list[i].init_func(ctx);
        duk_enum(ctx, -1, 0);
        while (duk_next(ctx, -1, 1)) {
          duk_put_prop(ctx, 1);
        }
        duk_pop_2(ctx);
        return 0;
      }
    }
    // We shouldn't reach here!
    return duk_error(ctx, DUK_ERR_ERROR, "Could not find module '%s'", id);
  } else { // Otherwise, it's in an external file
    push_file_contents(ctx, id);
    size_t len = strlen(id);
    size_t jsonext_len = strlen(".json");
    if (len >= jsonext_len && !strcmp(".json", id + len - jsonext_len)) {
      // Decode JSON file and place in exports
      duk_json_decode(ctx, -1);
      duk_enum(ctx, -1, 0);
      while (duk_next(ctx, -1, 1)) {
        duk_put_prop(ctx, 1);
      }
      duk_pop_2(ctx);
      return 0;
    } else { // A regular JS file
      return 1;
    }
  }
}
