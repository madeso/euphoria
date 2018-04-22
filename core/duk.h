#ifndef EUPHORIA_DUK_H
#define EUPHORIA_DUK_H


#include <string>
#include <functional>
#include <map>
#include <memory>
#include <vector>
#include <utility>

#include "core/assert.h"
#include "core/str.h"
#include "core/stringmerger.h"
#include "core/cpp.h"

// todo: add this as a option during build
#if 1
// provide c++ class name class when its not added to the registry
#define CLASS_ARG(x) , x
#define CLASS_NAME(x) x
#else
#define CLASS_ARG(x)
#define CLASS_NAME(x)
#endif

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

class Duk;

class Prototype
{
 public:
  Prototype(const std::string& n, void* p);

  std::string name;
  void*       prototype;
};

void*
GetVoidFunctionProperty(duk_context* ctx, const char* name);

void*
GetVoidProperty(duk_context* ctx, duk_idx_t index, const char* name);

void*
GetHiddenProperty(duk_context* ctx, duk_idx_t index, const std::string& name);

class Context
{
 public:
  Context(duk_context* c, Duk* d);

  int
  GetNumberOfArguments() const;

  // number argument

  bool
  IsNumber(int index) const;

  double
  GetNumber(int index);

  // string argument

  bool
  IsString(int index) const;

  std::string
  GetString(int index);

  // object argument

  bool
  IsObject(int index);

  Prototype*
  GetObjectType(int index);

  Prototype*
  TypeToProto(size_t id CLASS_ARG(const std::string& name));

  void*
  GetObjectPtr(int index);

  // function

  void*
  GetFunctionPtr(int index);

  bool
  IsFunction(int index);

  // array handling
  bool
  IsArray(int index);

  int
  GetArrayLength(int index);

  void
  GetArrayIndex(int array, int index);

  void
  StopArrayIndex();

  // return handling

  int
  ReturnVoid();

  int
  ReturnNumber(double num);

  int
  ReturnString(const std::string& str);

  int
  ReturnObject(
      void*          object,
      size_t         type,
      duk_c_function finalizer,
      void* data CLASS_ARG(const std::string& name));

  template <typename T>
  int
  ReturnFreeObject(T* t)
  {
    constexpr auto& cpptype = typeid(T);
    return ReturnObject(
        t, cpptype.hash_code(), nullptr, nullptr CLASS_ARG(cpptype.name()));
  }

  template <typename T>
  int
  ReturnObject(std::shared_ptr<T> t)
  {
    constexpr auto& cpptype = typeid(T);
    auto*           ptr     = new std::shared_ptr<T>(t);
    return ReturnObject(
        ptr->get(),
        cpptype.hash_code(),
        // this needs to conform to a duktape c function pointer
        // if needed move to global scope and make a template
        [](duk_context* ctx) -> duk_ret_t {
          void* data   = GetHiddenProperty(ctx, 0, "data");
          auto* my_ptr = reinterpret_cast<std::shared_ptr<T>*>(data);
          delete my_ptr;
          return 0;
        },
        ptr CLASS_ARG(cpptype.name()));
  }

  duk_context* ctx;
  Duk*         duk;
};

template <typename T>
void
PushVarImpl(Context* ctx, const T& arg)
{
  ctx->ReturnObject<T>(std::make_shared<T>(arg));
}

template <>
inline void
PushVarImpl(Context* ctx, const int& arg)
{
  ctx->ReturnNumber(arg);
}

template <>
inline void
PushVarImpl(Context* ctx, const std::string& arg)
{
  ctx->ReturnString(arg);
}


template <>
inline void
PushVarImpl(Context* ctx, const char* const& arg)
{
  ctx->ReturnString(arg);
}

template <typename T>
void
PushVar(Context* ctx, const T& arg)
{
  // *Impl functions here to support template-ing char* strings
  // https://stackoverflow.com/a/6559891/180307
  typedef typename ArrayToPointerDecay<T>::Type Type;
  PushVarImpl<Type>(ctx, arg);
}

class FunctionVar
{
 public:
  explicit FunctionVar(void* ptr = nullptr);

  bool
  IsValid() const;

  void
  StoreReference(Context* ctx);

  void
  BeginCall(Context* context) const;

  void
  CallFunction(Context* context, int arguments) const;

  void
  DoneFunction(Context* context) const;

  template <typename TReturn, typename... TArgs>
  TReturn
  Call(Context* context, TArgs... args) const;

  void* function;
};

class Overload
{
 public:
  Overload()          = default;
  virtual ~Overload() = default;

  // return empty string if matches
  // non-empty means match failed
  virtual std::string
  Matches(Context* ctx) = 0;

  virtual int
  Call(Context* ctx) = 0;

  virtual std::string
  Describe(Context* context) const = 0;
};

class Function
{
 public:
  Function()          = default;
  virtual ~Function() = default;

  std::vector<std::shared_ptr<Overload>> overloads;
};

std::string
ArgumentError(int arg, const std::string& err);

template <typename TT>
struct DukTemplate
{
  using T = typename std::decay<TT>::type;

  static std::string
  CanMatch(Context* ctx, int index, int arg)
  {
    if(ctx->IsObject(index))
    {
      const auto*     id      = ctx->GetObjectType(index);
      constexpr auto& cpptype = typeid(T);
      const auto*     self_type =
          ctx->TypeToProto(cpptype.hash_code() CLASS_ARG(cpptype.name()));
      ASSERT(self_type != nullptr);
      if(id == self_type)
      {
        return "";
      }
      else
      {
        return ArgumentError(
            arg,
            Str() << "expected " << self_type->name << " but got " << id->name);
      }
    }
    else
    {
      return ArgumentError(arg, "not a object");
    }
  }

  static T&
  Parse(Context* ctx, int index)
  {
    constexpr auto& cpptype = typeid(T);
    ASSERT(
        ctx->GetObjectType(index) ==
        ctx->TypeToProto(cpptype.hash_code() CLASS_ARG(cpptype.name())));
    return *static_cast<T*>(ctx->GetObjectPtr(index));
  }

  static std::string
  Name(Context* ctx)
  {
    constexpr auto& cpptype = typeid(T);
    auto*           proto =
        ctx->TypeToProto(cpptype.hash_code() CLASS_ARG(cpptype.name()));
    ASSERT(proto);
    return proto->name;
  }
};

template <>
struct DukTemplate<int>
{
  static std::string
  CanMatch(Context* ctx, int index, int arg)
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

  static int
  Parse(Context* ctx, int index)
  {
    return static_cast<int>(ctx->GetNumber(index));
  }

  static std::string
  Name(Context*)
  {
    return "int";
  }
};

template <>
struct DukTemplate<std::string>
{
  static std::string
  CanMatch(Context* ctx, int index, int arg)
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

  static std::string
  Parse(Context* ctx, int index)
  {
    return ctx->GetString(index);
  }

  static std::string
  Name(Context*)
  {
    return "string";
  }
};

template <>
struct DukTemplate<FunctionVar>
{
  static std::string
  CanMatch(Context* ctx, int index, int arg)
  {
    if(ctx->IsFunction(index))
    {
      return "";
    }
    else
    {
      return ArgumentError(arg, "not a function");
    }
  }

  static FunctionVar
  Parse(Context* ctx, int index)
  {
    ASSERT(ctx->IsObject(index));
    return FunctionVar{ctx->GetFunctionPtr(index)};
  }

  static std::string
  Name(Context*)
  {
    return "string";
  }
};

template <typename T>
struct DukTemplate<std::vector<T>>
{
  static std::string
  CanMatch(Context* ctx, int index, int arg)
  {
    if(ctx->IsArray(index))
    {
      const auto array_size = ctx->GetArrayLength(index);
      for(int i = 0; i < array_size; i += 1)
      {
        ctx->GetArrayIndex(index, i);
        const auto& match = DukTemplate<T>::CanMatch(ctx, -1, -1);
        ctx->StopArrayIndex();
        if(!match.empty())
        {
          return ArgumentError(
              arg,
              Str() << "array[" << i << "] (size: " << array_size
                    << ") has type error: "
                    << match);
        }
      }

      return "";
    }
    else
    {
      return ArgumentError(arg, "not a array");
    }
  }

  static std::vector<T>
  Parse(Context* ctx, int index)
  {
    std::vector<T> arr;

    const auto array_size = ctx->GetArrayLength(index);
    for(int i = 0; i < array_size; i += 1)
    {
      ctx->GetArrayIndex(index, i);
      arr.emplace_back(DukTemplate<T>::Parse(ctx, -1));
      ctx->StopArrayIndex();
    }

    return arr;
  }

  static std::string
  Name(Context* ctx)
  {
    return Str() << "[" << DukTemplate<T>::Name(ctx) << "]";
  }
};

template <typename TReturn, typename... TArgs>
TReturn
FunctionVar::Call(Context* context, TArgs... args) const
{
  BeginCall(context);

  int dummy[sizeof...(TArgs) + 1] = {0, (PushVar(context, args), 1)...};
  NotUsed(dummy);

  const auto arguments = sizeof...(TArgs);
  CallFunction(context, arguments);

  const auto match = DukTemplate<TReturn>::CanMatch(context, -1, 0);

  if(match.empty())
  {
    const TReturn ret = DukTemplate<TReturn>::Parse(context, -1);
    DoneFunction(context);
    return ret;
  }

  DoneFunction(context);

  // todo: handle invalid return type

  return Default<TReturn>();
}

template <typename Callback, typename... TArgs>
class GenericOverload : public Overload
{
 public:
  Callback callback;

  explicit GenericOverload(Callback c)
      : callback(c)
  {
  }

  template <std::size_t... I>
  std::string
  MatchesImpl(Context* ctx, std::index_sequence<I...>)
  {
    const auto passed_argument_count = ctx->GetNumberOfArguments();
    const int  argument_count        = sizeof...(TArgs);
    if(argument_count != passed_argument_count)
    {
      return Str{} << "expected " << argument_count << " argument(s) but got "
                   << passed_argument_count << ".";
    }

    const std::vector<std::string> matches{DukTemplate<TArgs>::CanMatch(
        ctx,
        -argument_count + static_cast<int>(I),
        static_cast<int>(I) + 1)...};
    for(const auto& m : matches)
    {
      if(!m.empty())
      {
        return m;
      }
    }

    return "";
  }

  std::string
  Matches(Context* ctx) override
  {
    return MatchesImpl(ctx, std::index_sequence_for<TArgs...>{});
  }

  template <std::size_t... I>
  int
  CallImpl(Context* ctx, std::index_sequence<I...>)
  {
    const int argument_count = sizeof...(TArgs);
    return callback(
        ctx,
        DukTemplate<TArgs>::Parse(
            ctx, -argument_count + static_cast<int>(I))...);
  }

  int
  Call(Context* ctx) override
  {
    return CallImpl(ctx, std::index_sequence_for<TArgs...>{});
  };

  std::string
  Describe(Context* ctx) const override
  {
    NotUsed(ctx);
    const std::vector<std::string> type_names = {
        DukTemplate<TArgs>::Name(ctx)...};
    return StringMerger::FunctionCall().Generate(type_names);
  }
};

class Bind
{
 public:
  template <typename... TArgs, typename Func>
  Bind&
  bind(Func callback)
  {
    overloads.emplace_back(
        std::make_shared<GenericOverload<Func, TArgs...>>(callback));
    return *this;
  }

  std::vector<std::shared_ptr<Overload>> overloads;
};

class ClassBinder
{
 public:
  explicit ClassBinder(size_t i);

  // todo: add constructor

  ClassBinder&
  SetConstructor(const Bind& bind);

  ClassBinder&
  AddMethod(const std::string& name, const Bind& bind);

  ClassBinder&
  AddProperty(const std::string& name, const Bind& get, const Bind& set);

  size_t id;
  Bind   constructor;
  std::vector<std::pair<std::string, Bind>> overloads;
  std::vector<std::tuple<std::string, Bind, Bind>> properties;
};

template <typename T>
ClassBinder
BindClass()
{
  return ClassBinder{typeid(T).hash_code()};
}

class Duk : private Context
{
 public:
  Duk();

  Context*
  AsContext();

  bool
  eval_string(
      const std::string& line,
      const std::string& file,
      std::string*       error,
      std::string*       output);

  void
  bind_print(std::function<void(const std::string&)> on_print);

  void
  BindGlobalFunction(const std::string& name, const Bind& overloads);

  void
  BindClass(const std::string& name, const ClassBinder& bind);

  ~Duk();

  Function*
  CreateFunction(const Bind& overloads);

  Prototype*
  TypeToProto(size_t id CLASS_ARG(const std::string& name));

  void
  StoreReference(void* p);

  std::function<void(const std::string&)> on_print;

  std::vector<std::shared_ptr<Function>> functions;

  std::map<size_t, std::shared_ptr<Prototype>> classIds;

  unsigned int reference_index = 0;
};


#endif  // EUPHORIA_DUK_H
