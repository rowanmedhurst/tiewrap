#ifndef MODULE_H
#define MODULE_H

#include <duktape.h>

struct c_module {
  const char *name;
  duk_ret_t (*init_func)(duk_context*);
};

static const struct c_module c_module_list[] = {
};

static const unsigned c_module_count = (sizeof(c_module_list) / sizeof(c_module_list[0]));

duk_ret_t push_file_contents(duk_context *ctx, const char *filename);
duk_ret_t cb_resolve_module(duk_context *ctx);
duk_ret_t cb_load_module(duk_context *ctx);

#endif