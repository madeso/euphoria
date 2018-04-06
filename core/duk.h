#ifndef EUPHORIA_DUK_H
#define EUPHORIA_DUK_H


#include <string>
#include <functional>
#include <map>
#include <memory>
#include <vector>
#include <utility>

// #include "duk_config.h"

extern "C" {
struct duk_hthread;
typedef struct duk_hthread duk_context;
}

class Duk;

class Context
{
 public:
  explicit Context(duk_context* c);

  int
  GetNumberOfArguments() const;

  duk_context* ctx;
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
  Describe() const = 0;
};

class Function
{
 public:
  Function()          = default;
  virtual ~Function() = default;

  std::vector<std::shared_ptr<Overload>> overloads;
};

template <typename T>
struct DukTemplate
{
  static std::string
  CanMatch(Context* ctx, int index);

  static T
  Parse(Context* ctx, int index);
};

template <typename>
void
Nop()
{
}

template <typename Callback, typename... TArgs>
class GenericOverload : public Overload
{
 public:
  Callback callback;

  GenericOverload(Callback c)
      : callback(c)
  {
  }

  std::string
  Matches(Context* ctx) override
  {
    const auto passed_argument_count = ctx->GetNumberOfArguments();
    const int  argument_count        = sizeof...(TArgs);
    if(argument_count != passed_argument_count)
    {
      return "invalid number of arguments passed";
    }

    int               i                           = 0;
    const std::string matches[argument_count + 1] = {
        "", DukTemplate<TArgs>::CanMatch(ctx, -argument_count + i++)...};
    for(const auto& m : matches)
    {
      if(!m.empty())
      {
        return m;
      }
    }

    return "";
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
  Describe() const override
  {
    return "not implemented";
  }
};

class FunctionBinder
{
 public:
  FunctionBinder(Duk* d, const std::string& n);
  ~FunctionBinder();

  FunctionBinder&
  add(std::shared_ptr<Overload> overload);

  template <typename... TArgs, typename Func>
  FunctionBinder&
  bind(Func callback)
  {
    return add(std::make_shared<GenericOverload<Func, TArgs...>>(callback));
  }

  Duk*                                   duk;
  std::string                            name;
  std::vector<std::shared_ptr<Overload>> overloads;
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
  bind(
      const std::string&                            name,
      const std::vector<std::shared_ptr<Overload>>& overloads);

  ~Duk();


  duk_context* ctx;

  std::function<void(const std::string&)> on_print;

  std::map<std::string, std::shared_ptr<Function>> functions;
};


#endif  // EUPHORIA_DUK_H
