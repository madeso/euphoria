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

// #include "duk_config.h"

extern "C" {
struct duk_hthread;
typedef struct duk_hthread duk_context;
}

class Duk;

class Context
{
 public:
  Context(duk_context* c, Duk* d);

  int
  GetNumberOfArguments() const;

  bool
  IsNumber(int index) const;

  double
  GetNumber(int index);

  bool
  IsString(int index) const;

  std::string
  GetString(int index);

  int
  ReturnVoid();

  int
  ReturnNumber(double num);

  int
  ReturnString(const std::string& str);

  bool
  IsObject(int index);

  size_t
  GetObjectId(int index);

  std::string
  TypeToString(size_t id);

  void*
  GetObjectPtr(int index);

  duk_context* ctx;
  Duk*         duk;
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

template <typename T>
struct DukTemplate
{
  static std::string
  CanMatch(Context* ctx, int index, int arg)
  {
    if(ctx->IsObject(index))
    {
      const auto id        = ctx->GetObjectId(index);
      const auto self_type = typeid(T).hash_code();
      if(id == self_type)
      {
        return "";
      }
      else
      {
        return ArgumentError(
            arg,
            Str() << "expected " << ctx->TypeToString(self_type) << " but got "
                  << ctx->TypeToString(id));
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
    ASSERT(ctx->GetObjectId(index) == typeid(T).hash_code());
    return *static_cast<T*>(ctx->GetObjectPtr(index));
  }

  static std::string
  Name(Context* ctx)
  {
    return ctx->TypeToString(typeid(T).hash_code());
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
  // todo: add properties

  ClassBinder&
  AddMethod(const std::string& name, const Bind& bind);

  size_t id;
  std::vector<std::pair<std::string, Bind>> overloads;
};

template <typename T>
ClassBinder
BindClass()
{
  return ClassBinder{typeid(T).hash_code()};
}

class Prototype
{
 public:
  Prototype(const std::string& n, void* p);

  std::string name;
  void*       prototype;
};

class Duk
{
 public:
  Duk();

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

  std::string
  TypeToString(size_t id);

  duk_context* ctx;

  std::function<void(const std::string&)> on_print;

  std::vector<std::shared_ptr<Function>> functions;

  std::map<size_t, Prototype> classIds;
};


#endif  // EUPHORIA_DUK_H
