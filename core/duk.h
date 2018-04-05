#ifndef EUPHORIA_DUK_H
#define EUPHORIA_DUK_H


#include <string>
#include <functional>
#include <map>
#include <memory>
#include <vector>

// #include "duk_config.h"

extern "C" {
struct duk_hthread;
typedef struct duk_hthread duk_context;
}

#define DUK_CALLBACK(TArgs) std::function<int(Context*, TArgs...)>

class Duk;

class Context
{
 public:
  int
  GetNumberOfArguments() const;
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

template <typename... TArgs>
class GenericOverload : public Overload
{
 public:
  typedef DUK_CALLBACK(TArgs) Callback;
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

    const std::string matches[argument_count] = {
        DukTemplate<TArgs>::CanMatch(ctx, -argument_count + 1)...};
    for(const auto& m : matches)
    {
      if(!m.empty())
      {
        return m;
      }
    }

    return "";
  }

  int
  Call(Context* ctx) override
  {
    const int argument_count = sizeof...(TArgs);
    return callback(
        ctx, DukTemplate<TArgs>::Parse(ctx, -argument_count + 1)...);
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
  FunctionBinder(Duk* duk, const std::string& name);
  ~FunctionBinder();

  FunctionBinder&
  add(std::shared_ptr<Overload> overload);

  template <typename... TArgs>
  FunctionBinder& operator-(DUK_CALLBACK(TArgs) callback)
  {
    return add(std::make_shared<GenericOverload<TArgs...>>(callback));
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
