#ifndef PROCESS_H
#define PROCESS_H

#include <duktape.h>

void duk_process_init(duk_context* ctx, int argc, char** argv);

#endif