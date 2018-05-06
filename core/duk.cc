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
    duk_enum(ctx, index, 0);
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

void
CollectDukError(duk_context* ctx, std::string* error)
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

////////////////////////////////////////////////////////////////////////////////

void*
GetVoidFunctionProperty(duk_context* ctx, const char* name)
{
  duk_push_current_function(ctx);
  duk_get_prop_string(ctx, -1, name);
  void* function = duk_to_pointer(ctx, -1);
  duk_pop(ctx);  // duk pointer
  duk_pop(ctx);  // current function
  return function;
}

void*
GetVoidProperty(duk_context* ctx, duk_idx_t index, const char* name)
{
  duk_get_prop_string(ctx, index, name);
  void* ptr = duk_get_pointer(ctx, -1);
  duk_pop(ctx);
  return ptr;
}

void*
GetHiddenProperty(duk_context* ctx, duk_idx_t index, const std::string& name)
{
  const std::string hidden_name = DUK_HIDDEN_SYMBOL("" + name);
  return GetVoidProperty(ctx, index, hidden_name.c_str());
}


////////////////////////////////////////////////////////////////////////////////

Context::Context(duk_context* c, Duk* d)
    : ctx(c)
    , duk(d)
{
}

int
Context::GetNumberOfArguments() const
{
  return duk_get_top(ctx);
}

////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////

bool
Context::IsObject(int index)
{
  return duk_is_object(ctx, index) == 1;
}

Prototype*
Context::GetObjectType(int index)
{
  duk_get_prop_string(ctx, index, DUK_HIDDEN_SYMBOL("type"));
  const auto ptr = duk_get_pointer(ctx, -1);
  duk_pop(ctx);
  return static_cast<Prototype*>(ptr);
}

Prototype*
Context::TypeToProto(size_t id CLASS_ARG(const std::string& name))
{
  return duk->TypeToProto(id CLASS_ARG(name));
}

void*
Context::GetObjectPtr(int index)
{
  duk_get_prop_string(ctx, index, DUK_HIDDEN_SYMBOL("ptr"));
  void* data = duk_get_pointer(ctx, -1);
  duk_pop(ctx);
  return data;
}

////////////////////////////////////////////////////////////////////////////////

void*
Context::GetFunctionPtr(int index)
{
  return duk_get_heapptr(ctx, index);
}

bool
Context::IsFunction(int index)
{
  return duk_is_function(ctx, index) == 1;
}

////////////////////////////////////////////////////////////////////////////////

bool
Context::IsArray(int index)
{
  return duk_is_array(ctx, index) == 1;
}

int
Context::GetArrayLength(int index)
{
  // duk_inspect_value(ctx, index);
  duk_get_prop_string(ctx, index, "length");
  const auto size = duk_to_int(ctx, -1);
  // duk_pop_2(ctx);
  duk_pop(ctx);
  return size;
}

void
Context::GetArrayIndex(int array, int index)
{
  const auto put = duk_get_prop_index(ctx, array, index);
  ASSERT(put == 1);
}

void
Context::StopArrayIndex()
{
  duk_pop(ctx);
}

int
Context::PushArray()
{
  return duk_push_array(ctx);
}

void
Context::PutArrayIndex(int arr, unsigned int i)
{
  duk_put_prop_index(ctx, arr, i);
}

////////////////////////////////////////////////////////////////////////////////

int
Context::ReturnVoid()
{
  return 0;
}

int
Context::Return(int value)
{
  return ReturnNumber(value);
}

int
Context::Return(const std::string& value)
{
  return ReturnString(value);
}

int
Context::Return(DukValue val)
{
  duk_push_heapptr(ctx, val.ptr);
  return 1;
}

int
Context::ReturnNumber(double num)
{
  duk_push_number(ctx, num);
  return 1;
}

int
Context::ReturnString(const std::string& str)
{
  duk_push_string(ctx, str.c_str());
  return 1;
}

int
Context::ReturnObject(
    void*          object,
    size_t         type,
    duk_c_function finalizer,
    void* data CLASS_ARG(const std::string& name))
{
  if(object == nullptr)
  {
    duk_push_null(ctx);
    return 1;
  }
  Prototype* proto     = duk->TypeToProto(type CLASS_ARG(name));
  const auto object_id = duk_push_object(ctx);  // object

  // prototype
  duk_push_heapptr(ctx, proto->prototype);  // object proto
  duk_set_prototype(ctx, object_id);        // object

  // type id
  duk_push_pointer(ctx, proto);                                    // object id
  duk_put_prop_string(ctx, object_id, DUK_HIDDEN_SYMBOL("type"));  // object

  // object ptr
  duk_push_pointer(ctx, object);                                  // object ptr
  duk_put_prop_string(ctx, object_id, DUK_HIDDEN_SYMBOL("ptr"));  // object

  if(finalizer)
  {
    duk_push_pointer(ctx, data);  // object data
    duk_put_prop_string(ctx, object_id, DUK_HIDDEN_SYMBOL("data"));  // object

    duk_push_c_function(ctx, finalizer, 1);  // object finalzer
    duk_set_finalizer(ctx, object_id);       // object
  }

  return 1;
}

////////////////////////////////////////////////////////////////////////////////

FunctionVar::FunctionVar(void* ptr)
    : function(ptr)
{
}

bool
FunctionVar::IsValid() const
{
  return function != nullptr;
}

void
FunctionVar::StoreReference(Context* ctx)
{
  ctx->duk->StoreReference(function);
}

void
FunctionVar::BeginCall(Context* context) const
{
  ASSERT(IsValid());
  duk_push_heapptr(context->ctx, function);
}

bool
FunctionVar::CallFunction(Context* context, int arguments) const
{
  ASSERT(IsValid());
  return duk_pcall(context->ctx, arguments) == DUK_EXEC_SUCCESS;
}

std::string
FunctionVar::CollectError(Context* context) const
{
  std::string error = "unknown error";
  CollectDukError(context->ctx, &error);
  duk_pop(context->ctx);
  return error;
}

void
FunctionVar::DoneFunction(Context* context) const
{
  ASSERT(IsValid());
  duk_pop(context->ctx);
}

////////////////////////////////////////////////////////////////////////////////

DukValue::DukValue()
    : ptr(nullptr)
{
}

DukValue::DukValue(void* p)
    : ptr(p)
{
}

bool
DukValue::IsValid() const
{
  return ptr != nullptr;
}

void
DukValue::SetFreeImpl(
    Duk*               duk,
    const std::string& name,
    void*              object,
    size_t type CLASS_ARG(const std::string& classname))
{
  Context* ctx = duk->AsContext();
  duk_push_heapptr(ctx->ctx, ptr);
  ctx->ReturnObject(object, type, nullptr, nullptr CLASS_ARG(classname));
  duk_put_prop_string(ctx->ctx, -2, name.c_str());
  duk_pop(ctx->ctx);
}

////////////////////////////////////////////////////////////////////////////////


std::string
ArgumentError(int arg, const std::string& err)
{
  if(arg < 0)
  {
    return err;
  }
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
  return Str() << arg << th << " arg, " << err;
}

////////////////////////////////////////////////////////////////////////////////

ClassBinder::ClassBinder(size_t i)
    : id(i)
{
}

ClassBinder&
ClassBinder::SetConstructor(const Bind& bind)
{
  constructor = bind;
  return *this;
}

ClassBinder&
ClassBinder::AddMethod(const std::string& name, const Bind& bind)
{
  overloads.emplace_back(std::make_pair(name, bind));
  return *this;
}

ClassBinder&
ClassBinder::AddProperty(
    const std::string& name, const Bind& get, const Bind& set)
{
  properties.push_back(std::make_tuple(name, get, set));
  return *this;
}

////////////////////////////////////////////////////////////////////////////////

ObjectBinder&
ObjectBinder::AddFunction(const std::string& name, const Bind& bind)
{
  functions.emplace_back(std::make_pair(name, bind));
  return *this;
}

ObjectBinder
BindObject()
{
  return ObjectBinder{};
}

////////////////////////////////////////////////////////////////////////////////

Duk::Duk()
    : Context(
          duk_create_heap(nullptr, nullptr, nullptr, nullptr, FatalHandler),
          this)
{
}

DukValue
Duk::CreateGlobal(const std::string& name)
{
  duk_push_object(ctx);
  DukValue val = DukValue{duk_get_heapptr(ctx, -1)};
  duk_put_global_string(ctx, name.c_str());
  return val;
}

Context*
Duk::AsContext()
{
  return this;
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
        CollectDukError(ctx, error);
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
    const int index = -args + i;
#if 1
    const std::string err = Str() << GetDukType(ctx, index) << "="
                                  << to_string(ctx->ctx, index);
    types.emplace_back(err);
#else
    types.emplace_back(GetDukType(ctx, index));
#endif
  }
  return StringMerger::FunctionCall().Generate(types);
}

template <typename T>
T*
GetFunctionProperty(duk_context* ctx, const char* name)
{
  void* ptr = GetVoidFunctionProperty(ctx, name);
  return reinterpret_cast<T*>(ptr);
}

template <bool TPushThis, bool TConstructor>
int
duk_generic_function_callback(duk_context* ctx)
{
  if(TConstructor)
  {
    if(!duk_is_constructor_call(ctx))
    {
      if(TPushThis)
      {
        return duk_type_error(ctx, "%s", "Not a method call");
      }
      else
      {
        return duk_type_error(ctx, "%s", "Not a function call");
      }
    }
  }
  else
  {
    if(duk_is_constructor_call(ctx))
    {
      return duk_type_error(ctx, "%s", "Not a constructor call");
    }
  }

  auto* function =
      GetFunctionProperty<Function>(ctx, DUK_HIDDEN_SYMBOL("func"));
  auto* duk = GetFunctionProperty<Duk>(ctx, DUK_HIDDEN_SYMBOL("duk"));

  Context context{ctx, duk};

  if(TPushThis)
  {
    const auto arguments = duk_get_top(ctx);
    duk_push_this(ctx);
    if(arguments != 0)
    {
      // don't insert if there are 0 arguments
      duk_insert(ctx, -(arguments + 1));
    }
  }

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
      non_matches.emplace_back(
          Str() << overload->Describe(&context) << ": " << match);
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
PlaceFunctionOnStack(
    duk_context*   ctx,
    Function*      function,
    duk_c_function fun,
    Duk*           duk,
    int            arguments = DUK_VARARGS)
{
  duk_push_c_function(ctx, fun, arguments);  // fun

  duk_push_pointer(ctx, function);                          // fun pointer
  duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("func"));  // fun

  duk_push_pointer(ctx, duk);                              // fun pointer
  duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("duk"));  // fun
}

Prototype::Prototype(const std::string& n, void* p)
    : name(n)
    , prototype(p)
{
}

void
Duk::BindGlobalFunction(const std::string& name, const Bind& bind)
{
  PlaceFunctionOnStack(
      ctx,
      CreateFunction(bind),
      duk_generic_function_callback<false, false>,
      this);

  const auto function_added = duk_put_global_string(ctx, name.c_str());
  ASSERTX(function_added == 1, function_added);
}

void
Duk::BindObject(const std::string& name, const ObjectBinder& bind)
{
  const auto object_index = duk_push_object(ctx);

  for(const auto& func : bind.functions)
  {
    PlaceFunctionOnStack(
        ctx,
        CreateFunction(func.second),
        duk_generic_function_callback<false, false>,
        this);

    const auto function_added =
        duk_put_prop_string(ctx, object_index, func.first.c_str());
    ASSERTX(function_added == 1, function_added);
  }

  const auto object_added = duk_put_global_string(ctx, name.c_str());
  ASSERTX(object_added == 1, object_added);
}

void
Duk::BindClass(const std::string& name, const ClassBinder& bind)
{
  if(!bind.constructor.overloads.empty())
  {
    // constructor
    PlaceFunctionOnStack(
        ctx,
        CreateFunction(bind.constructor),
        duk_generic_function_callback<false, true>,
        this);  // constructor
    const auto constructor_added = duk_put_global_string(ctx, name.c_str());
    ASSERTX(constructor_added == 1, constructor_added);
  }

  // use duk_push_bare_object?
  const auto prototype_index = duk_push_object(ctx);  // prototype

  for(const auto& func : bind.overloads)
  {
    PlaceFunctionOnStack(
        ctx,
        CreateFunction(func.second),
        duk_generic_function_callback<true, false>,
        this);  // proto func
    const auto function_added =
        duk_put_prop_string(ctx, prototype_index, func.first.c_str());  // proto
    ASSERTX(function_added == 1, function_added);
  }

  for(const auto& prop : bind.properties)
  {
    const auto& propname = std::get<0>(prop);
    const auto& getter   = std::get<1>(prop);
    const auto& setter   = std::get<2>(prop);

    duk_uint_t flags = 0;

    duk_push_string(ctx, propname.c_str());

    if(!getter.overloads.empty())
    {
      PlaceFunctionOnStack(
          ctx,
          CreateFunction(getter),
          duk_generic_function_callback<true, false>,
          this,
          0);
      flags |= DUK_DEFPROP_HAVE_GETTER;
    }
    if(!setter.overloads.empty())
    {
      PlaceFunctionOnStack(
          ctx,
          CreateFunction(setter),
          duk_generic_function_callback<true, false>,
          this,
          1);
      flags |= DUK_DEFPROP_HAVE_SETTER;
    }
    // a getter, setter or both must be set
    ASSERTX(flags != 0, flags);
    duk_def_prop(ctx, prototype_index, flags);
  }

  void*             prototype  = duk_get_heapptr(ctx, -1);
  const std::string proto_name = Str() << DUK_HIDDEN_SYMBOL("proto") << name;
  duk_put_global_string(ctx, proto_name.c_str());  // empty stack

  classIds.insert(
      std::make_pair(bind.id, std::make_shared<Prototype>(name, prototype)));
}

Function*
Duk::CreateFunction(const Bind& bind)
{
  // create new function object
  auto func       = std::make_shared<Function>();
  func->overloads = bind.overloads;
  functions.emplace_back(func);
  return func.get();
}

Duk::~Duk()
{
  duk_destroy_heap(ctx);
}

Prototype*
Duk::TypeToProto(size_t id CLASS_ARG(const std::string& name))
{
  const auto found = classIds.find(id);
  if(found == classIds.end())
  {
    const std::string error = Str()
                              << "class not added" CLASS_NAME(": " << name);
    DIE(error.c_str());
    return nullptr;
  }
  else
  {
    return found->second.get();
  }
}

void
Duk::StoreReference(void* p)
{
  // todo: store references in some sub object instead of directly at root?
  duk_push_heap_stash(ctx);                      // heap
  duk_push_heapptr(ctx, p);                      // heap ptr
  duk_put_prop_index(ctx, -2, reference_index);  // heap
  duk_pop(ctx);                                  //
  reference_index += 1;
}
