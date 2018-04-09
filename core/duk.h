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

class Prototype
{
 public:
  Prototype(const std::string& n, void* p);

  std::string name;
  void*       prototype;
};


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

  int
  ReturnFreeObject(void* object, size_t type);

  template <typename T>
  int
  ReturnFreeObject(T* t)
  {
    return ReturnFreeObject(t, typeid(T).hash_code());
  }

  bool
  IsObject(int index);

  Prototype*
  GetObjectType(int index);

  Prototype*
  TypeToProto(size_t id);

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

template <typename TT>
struct DukTemplate
{
  using T = typename std::decay<TT>::type;

  static std::string
  CanMatch(Context* ctx, int index, int arg)
  {
    if(ctx->IsObject(index))
    {
      const auto* id        = ctx->GetObjectType(index);
      const auto* self_type = ctx->TypeToProto(typeid(T).hash_code());
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
    ASSERT(
        ctx->GetObjectType(index) == ctx->TypeToProto(typeid(T).hash_code()));
    return *static_cast<T*>(ctx->GetObjectPtr(index));
  }

  static std::string
  Name(Context* ctx)
  {
    auto* proto = ctx->TypeToProto(typeid(T).hash_code());
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

  ClassBinder&
  AddProperty(const std::string& name, const Bind& get, const Bind& set);

  size_t id;
  std::vector<std::pair<std::string, Bind>> overloads;
  std::vector<std::tuple<std::string, Bind, Bind>> properties;
};

template <typename T>
ClassBinder
BindClass()
{
  return ClassBinder{typeid(T).hash_code()};
}

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

  Prototype*
  TypeToProto(size_t id);

  duk_context* ctx;

  std::function<void(const std::string&)> on_print;

  std::vector<std::shared_ptr<Function>> functions;

  std::map<size_t, std::shared_ptr<Prototype>> classIds;
};


#endif  // EUPHORIA_DUK_H
