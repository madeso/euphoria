#ifndef EUPHORIA_DUK_H
#define EUPHORIA_DUK_H


#include <string>
#include <functional>
#include <map>
#include <memory>
#include <vector>
#include <utility>

#include "core/str.h"
#include "stringmerger.h"

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
  CanMatch(Context* ctx, int index, int number);

  static T
  Parse(Context* ctx, int index);

  static std::string
  Name();
};

template <typename Callback, typename... TArgs>
class GenericOverload : public Overload
{
 public:
  Callback callback;

  GenericOverload(Callback c)
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
  Describe() const override
  {
    const std::vector<std::string> type_names = {DukTemplate<TArgs>::Name()...};
    return StringMerger::FunctionCall().Generate(type_names);
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
