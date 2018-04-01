#include "core/duk.h"

#include <sstream>

#include "core/log.h"
#include "core/assert.h"
#include "core/str.h"
#include "core/stringmerger.h"

#include "duktape.h"

LOG_SPECIFY_DEFAULT_LOGGER("engine.duk")

std::vector<std::string>
collect_types(duk_context* ctx, int index)
{
  std::vector<std::string> types;

  if(duk_is_array(ctx, index))
  {
    types.emplace_back("array");
  }
  if(duk_is_boolean(ctx, index))
  {
    types.emplace_back("boolean");
  }
  if(duk_is_bound_function(ctx, index))
  {
    types.emplace_back("bound function");
  }
  if(duk_is_buffer(ctx, index))
  {
    types.emplace_back("buffer");
  }
  if(duk_is_buffer_data(ctx, index))
  {
    types.emplace_back("buffer data");
  }
  if(duk_is_c_function(ctx, index))
  {
    types.emplace_back("C function");
  }
  if(duk_is_callable(ctx, index))
  {
    types.emplace_back("callable");
  }
  if(duk_is_constructable(ctx, index))
  {
    types.emplace_back("constructable");
  }
  if(duk_is_dynamic_buffer(ctx, index))
  {
    types.emplace_back("dynamic buffer");
  }
  if(duk_is_ecmascript_function(ctx, index))
  {
    types.emplace_back("ecmascript function");
  }
  if(duk_is_eval_error(ctx, index))
  {
    types.emplace_back("eval error");
  }
  if(duk_is_fixed_buffer(ctx, index))
  {
    types.emplace_back("fixed buffer");
  }
  if(duk_is_function(ctx, index))
  {
    types.emplace_back("function");
  }
  if(duk_is_nan(ctx, index))
  {
    types.emplace_back("NAN");
  }
  if(duk_is_null(ctx, index))
  {
    types.emplace_back("NULL");
  }
  if(duk_is_null_or_undefined(ctx, index))
  {
    types.emplace_back("null/undefined");
  }
  if(duk_is_object(ctx, index))
  {
    types.emplace_back("object");
  }
  if(duk_is_object_coercible(ctx, index))
  {
    types.emplace_back("object coercible");
  }
  if(duk_is_pointer(ctx, index))
  {
    types.emplace_back("pointer");
  }
  if(duk_is_primitive(ctx, index))
  {
    types.emplace_back("primitive");
  }
  if(duk_is_range_error(ctx, index))
  {
    types.emplace_back("range error");
  }
  if(duk_is_reference_error(ctx, index))
  {
    types.emplace_back("reference error");
  }
  if(duk_is_string(ctx, index))
  {
    types.emplace_back("string");
  }
  if(duk_is_symbol(ctx, index))
  {
    types.emplace_back("symbol");
  }
  if(duk_is_syntax_error(ctx, index))
  {
    types.emplace_back("syntax error");
  }
  if(duk_is_thread(ctx, index))
  {
    types.emplace_back("thread");
  }
  if(duk_is_undefined(ctx, index))
  {
    types.emplace_back("undefined");
  }
  if(duk_is_uri_error(ctx, index))
  {
    types.emplace_back("uri error");
  }

  return types;
}

std::string
to_string(duk_context* ctx, int index)
{
  if(duk_is_number(ctx, index))
  {
    std::stringstream ss;
    ss << duk_get_number(ctx, index);
    return ss.str();
  }

  if(duk_is_string(ctx, index))
  {
    return duk_safe_to_string(ctx, index);
  }

  const auto types = collect_types(ctx, index);

  std::vector<std::string> values;
  if(duk_is_object(ctx, index))
  {
    duk_enum(ctx, index, DUK_ENUM_INCLUDE_NONENUMERABLE);
    while(duk_next(ctx, -1, 1))
    {
      values.emplace_back(
          Str() << duk_to_string(ctx, -2) << ": " << duk_to_string(ctx, -1));
      duk_pop_2(ctx);
    }
    duk_pop(ctx);  // enum object
  }

  std::stringstream ss;
  ss << StringMerger::EnglishAnd().Generate(types) << ": "
     << StringMerger::Array().Generate(values);
  return ss.str();
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
}

bool
Duk::eval_string(
    const std::string& line,
    const std::string& file,
    std::string*       error,
    std::string*       output)
{
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
        *output = to_string(ctx, -1);  // duk_safe_to_string(ctx, -1);
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
