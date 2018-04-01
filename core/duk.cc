#include "core/duk.h"

#include <sstream>

#include "core/log.h"
#include "core/assert.h"

#include "engine/dukprint.h"

#include "duktape.h"

LOG_SPECIFY_DEFAULT_LOGGER("engine.duk")

std::string
to_string(duk_context* ctx, int index)
{
  if(duk_is_array(ctx, index))
  {
    return "array";
  }
  else if(duk_is_boolean(ctx, index))
  {
    return "boolean";
  }
  else if(duk_is_bound_function(ctx, index))
  {
    return "bound function";
  }
  else if(duk_is_buffer(ctx, index))
  {
    return "buffer";
  }
  else if(duk_is_buffer_data(ctx, index))
  {
    return "buffer data";
  }
  else if(duk_is_c_function(ctx, index))
  {
    return "C function";
  }
  else if(duk_is_callable(ctx, index))
  {
    return "callable";
  }
  else if(duk_is_constructable(ctx, index))
  {
    return "constructable";
  }
  else if(duk_is_dynamic_buffer(ctx, index))
  {
    return "dynamic buffer";
  }
  else if(duk_is_ecmascript_function(ctx, index))
  {
    return "ecmascript function";
  }
  else if(duk_is_eval_error(ctx, index))
  {
    return "eval error";
  }
  else if(duk_is_fixed_buffer(ctx, index))
  {
    return "fixed buffer";
  }
  else if(duk_is_function(ctx, index))
  {
    return "function";
  }
  else if(duk_is_nan(ctx, index))
  {
    return "NAN";
  }
  else if(duk_is_null(ctx, index))
  {
    return "NULL";
  }
  else if(duk_is_null_or_undefined(ctx, index))
  {
    return "null/undefined";
  }
  else if(duk_is_number(ctx, index))
  {
    std::stringstream ss;
    ss << duk_get_number(ctx, index);
    return ss.str();
  }
  else if(duk_is_object(ctx, index))
  {
    return "object";
  }
  else if(duk_is_object_coercible(ctx, index))
  {
    return "object coercible";
  }
  else if(duk_is_pointer(ctx, index))
  {
    return "pointer";
  }
  else if(duk_is_primitive(ctx, index))
  {
    return "primitive";
  }
  else if(duk_is_range_error(ctx, index))
  {
    return "range error";
  }
  else if(duk_is_reference_error(ctx, index))
  {
    return "reference error";
  }
  else if(duk_is_string(ctx, index))
  {
    return "string";
  }
  else if(duk_is_symbol(ctx, index))
  {
    return "symbol";
  }
  else if(duk_is_syntax_error(ctx, index))
  {
    return "syntax error";
  }
  else if(duk_is_thread(ctx, index))
  {
    return "thread";
  }
  else if(duk_is_undefined(ctx, index))
  {
    return "undefined";
  }
  else if(duk_is_uri_error(ctx, index))
  {
    return "uri error";
  }
  else
  {
    return "unknown";
  }
}

void
FatalHandler(void*, const char* msg)
{
  LOG_ERROR("*** FATAL ERROR: " << (msg ? msg : "no message"));
  DIE("FATAL JS ERROR");
  abort();
}

Duk::Duk()
{
  ctx = duk_create_heap(nullptr, nullptr, nullptr, nullptr, FatalHandler);
  // ctx = duk_create_heap_default();

  AddPrint(this);
}

bool
Duk::eval_string(
    const std::string& line, std::string* error, std::string* output)
{
  const std::string file = "input.js";
  duk_push_string(ctx, line.c_str());
  duk_push_string(ctx, file.c_str());

  const auto compile_result = duk_pcompile(ctx, 0);
  bool       ok             = false;
  if(compile_result != 0)
  {
    if(error)
    {
      *error = duk_safe_to_string(ctx, -1);
    }
    ok = false;
  }
  else
  {
    const auto call_result = duk_pcall(ctx, 0);
    if(call_result != 0)
    {
      if(error)
      {
        // stolen from duk documentation: http://duktape.org/api.html#duk_pcall
        if(duk_is_error(ctx, -1))
        {
          // todo: add better error check
          /* Accessing .stack might cause an error to be thrown, so wrap this
           * access in a duk_safe_call() if it matters.
           */
          duk_get_prop_string(ctx, -1, "stack");
          *error = duk_safe_to_string(ctx, -1);
          duk_pop(ctx);
        }
        else
        {
          *error = duk_safe_to_string(ctx, -1);
        }
      }
      ok = false;
    }
    else
    {
      if(output)
      {
        *output = duk_safe_to_string(ctx, -1);
      }
      ok = true;
    }
  }
  duk_pop(ctx);

  return ok;
}

Duk::~Duk()
{
  duk_destroy_heap(ctx);
}
