#include "core/duk.h"

#include <sstream>

#include "core/log.h"
#include "core/assert.h"
#include "core/str.h"
#include "core/stringmerger.h"
#include "core/stringutils.h"

#include "duktape.h"

LOG_SPECIFY_DEFAULT_LOGGER("engine.duk")

////////////////////////////////////////////////////////////////////////////////

Context::Context(duk_context* c)
    : ctx(c)
{
}

int
Context::GetNumberOfArguments() const
{
  return duk_get_top(ctx);
}

bool
Context::IsNumber(int index) const
{
  return duk_is_number(ctx, index) == 1;
}

double
Context::GetNumber(int index)
{
  return duk_get_number(ctx, index);
}

bool
Context::IsString(int index) const
{
  return duk_is_string(ctx, index) == 1;
}

std::string
Context::GetString(int index)
{
  return duk_get_string(ctx, index);
}

int
Context::ReturnVoid()
{
  return 0;
}

////////////////////////////////////////////////////////////////////////////////

std::string
ArgumentError(int arg, const std::string& err)
{
  std::string th;
  switch(arg % 10)
  {
    case 1:
      th = "st";
      break;
    case 2:
      th = "nd";
      break;
    case 3:
      th = "rd";
      break;
    default:
      th = "th";
      break;
  }
  return Str() << arg << th << " argument is not a string";
}

////////////////////////////////////////////////////////////////////////////////

template <>
std::string
DukTemplate<int>::CanMatch(Context* ctx, int index, int arg)
{
  if(ctx->IsNumber(index))
  {
    return "";
  }
  else
  {
    return ArgumentError(arg, "not a number");
  }
}

template <>
int
DukTemplate<int>::Parse(Context* ctx, int index)
{
  return static_cast<int>(ctx->GetNumber(index));
}

template <>
std::string
DukTemplate<int>::Name()
{
  return "int";
}

////////////////////////////////////////////////////////////////////////////////

template <>
std::string
DukTemplate<std::string>::CanMatch(Context* ctx, int index, int arg)
{
  if(ctx->IsString(index))
  {
    return "";
  }
  else
  {
    return ArgumentError(arg, "not a string");
  }
}

template <>
std::string
DukTemplate<std::string>::Parse(Context* ctx, int index)
{
  return ctx->GetString(index);
}

template <>
std::string
DukTemplate<std::string>::Name()
{
  return "string";
}

////////////////////////////////////////////////////////////////////////////////

FunctionBinder::FunctionBinder(Duk* d, const std::string& n)
    : duk(d)
    , name(n)
{
}

FunctionBinder::~FunctionBinder()
{
  duk->bind(name, overloads);
}

FunctionBinder&
FunctionBinder::add(std::shared_ptr<Overload> overload)
{
  overloads.push_back(overload);
  return *this;
}

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
    if(call_result != DUK_EXEC_SUCCESS)
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
          *error = to_string(ctx, -1);
        }

        if(Trim(*error).empty())
        {
          *error = "<unknown error>";
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


int
duk_print_function_callback(duk_context* ctx)
{
  if(duk_is_constructor_call(ctx))
  {
    return duk_type_error(ctx, "%s", "Not a constructor call");
  }

  duk_push_current_function(ctx);
  duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("duk"));
  auto* duk = reinterpret_cast<Duk*>(duk_to_pointer(ctx, -1));
  duk_pop(ctx);  // duk pointer
  duk_pop(ctx);  // current function

  const int number_of_arguments = duk_get_top(ctx);

  std::stringstream ss;

  auto first = true;

  for(int arg = number_of_arguments; arg > 0; arg -= 1)
  {
    if(first)
    {
      first = false;
    }
    else
    {
      ss << " ";
    }
    ss << to_string(ctx, -arg);
  }

  duk->on_print(ss.str());

  return 0;
}

void
Duk::bind_print(std::function<void(const std::string&)> on_print)
{
  this->on_print = on_print;

  duk_push_c_function(ctx, duk_print_function_callback, DUK_VARARGS);  // fun
  duk_push_pointer(ctx, this);                             // fun pointer
  duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("duk"));  // fun

  const auto function_added = duk_put_global_string(ctx, "print");
  ASSERTX(function_added == 1, function_added);
}

std::string
GetDukType(Context* a, int index)
{
  duk_context* ctx = a->ctx;
  if(duk_is_number(ctx, index))
  {
    return "number";
  }
  if(duk_is_object(ctx, index))
  {
    return "object";
  }
  if(duk_is_string(ctx, index))
  {
    return "string";
  }
  return "???";
}

std::string
DescribeArguments(Context* ctx)
{
  const int                args = ctx->GetNumberOfArguments();
  std::vector<std::string> types;
  for(int i = 0; i < args; i += 1)
  {
    types.emplace_back(GetDukType(ctx, -args + i));
  }
  return StringMerger::FunctionCall().Generate(types);
}

int
duk_generic_function_callback(duk_context* ctx)
{
  if(duk_is_constructor_call(ctx))
  {
    return duk_type_error(ctx, "%s", "Not a constructor call");
  }

  duk_push_current_function(ctx);
  duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("func"));
  auto* function = reinterpret_cast<Function*>(duk_to_pointer(ctx, -1));
  duk_pop(ctx);  // duk pointer
  duk_pop(ctx);  // current function

  Context context{ctx};

  // const int                number_of_arguments = duk_get_top(ctx);
  std::vector<std::string> non_matches;

  std::vector<std::shared_ptr<Overload>> matched;
  for(auto& overload : function->overloads)
  {
    const auto match = overload->Matches(&context);
    if(match.empty())
    {
      matched.emplace_back(overload);
    }
    else
    {
      non_matches.emplace_back(Str() << overload->Describe() << ": " << match);
    }
  }

  if(matched.empty())
  {
    const auto arguments = DescribeArguments(&context);
    const auto described = StringMerger::DashForEach().Generate(non_matches);
    return duk_type_error(
        ctx,
        "No matches found for %s, tried %s",
        arguments.c_str(),
        described.c_str());
  }


  if(matched.size() != 1)
  {
    const auto arguments = DescribeArguments(&context);
    const auto described = StringMerger::EnglishAnd().Generate(non_matches);
    return duk_type_error(
        ctx,
        "Found several matches for(%s), tried %s",
        arguments.c_str(),
        described.c_str());
  }

  return matched[0]->Call(&context);
}

void
Duk::bind(
    const std::string&                            name,
    const std::vector<std::shared_ptr<Overload>>& overloads)
{
  // find out if we have made this function before
  ASSERT(functions.find(name) == functions.end());

  // create new function object
  auto func       = std::make_shared<Function>();
  func->overloads = overloads;
  functions.insert(std::make_pair(name, func));

  // bind duk function
  duk_push_c_function(ctx, duk_generic_function_callback, DUK_VARARGS);  // fun
  duk_push_pointer(ctx, func.get());                        // fun pointer
  duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("func"));  // fun

  const auto function_added = duk_put_global_string(ctx, name.c_str());
  ASSERTX(function_added == 1, function_added);
}

Duk::~Duk()
{
  duk_destroy_heap(ctx);
}
