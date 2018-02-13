#include "engine/duk.h"

#include <sstream>

#include "duktape.h"

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

Duk::Duk()
{
  ctx = duk_create_heap_default();
}

bool
Duk::eval_string(
    const std::string& line, std::string* error, std::string* output)
{
  const auto r  = duk_peval_string(ctx, line.c_str());
  bool       ok = false;
  if(r != 0)
  {
    if(error)
    {
      *error = duk_safe_to_string(ctx, -1);
    }
    ok = false;
  }
  else
  {
    if(output)
    {
      *output = to_string(ctx, -1);
    }
    ok = true;
  }
  duk_pop(ctx);

  return ok;
}

Duk::~Duk()
{
  duk_destroy_heap(ctx);
}
