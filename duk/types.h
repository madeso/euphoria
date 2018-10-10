#ifndef EUPHORIA_FAKE_DUKTAPE_H
#define EUPHORIA_FAKE_DUKTAPE_H

// #include "duk_config.h"

extern "C" {
struct duk_hthread;
typedef struct duk_hthread duk_context;
typedef int                duk_small_int_t;
typedef duk_small_int_t    duk_ret_t;
typedef duk_ret_t (*duk_c_function)(duk_context* ctx);
typedef int       duk_int_t;
typedef duk_int_t duk_idx_t;
}

#endif  // EUPHORIA_FAKE_DUKTAPE_H
