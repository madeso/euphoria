#ifndef EUPHORIA_DUK_FUNCTION_H
#define EUPHORIA_DUK_FUNCTION_H

#include <string>
#include <vector>

class Context;

#include "core/cpp.h"
#include "core/str.h"

#include "duk/template.h"
#include "duk/pushvarimpl.h"


// todo: rename to DukFunc
class FunctionVar
{
 public:
  explicit FunctionVar(void* ptr = nullptr);

  bool
  IsValid() const;

  // todo: remove this and make the function ref counted instead
  void
  StoreReference(Context* ctx);

  void
  BeginCall(Context* context) const;

  bool
  CallFunction(Context* context, int arguments) const;

  std::string
  CollectError(Context* context) const;

  void
  DoneFunction(Context* context) const;

  template <typename... TArgs>
  bool
  SubCall(Context* context, TArgs... args) const;

  template <typename TReturn, typename... TArgs>
  TReturn
  Call(Context* context, TArgs... args) const;

  template <typename... TArgs>
  void
  VoidCall(Context* context, TArgs... args) const;

  void* function;
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
    return "function";
  }
};


template <typename... TArgs>
bool
FunctionVar::SubCall(Context* context, TArgs... args) const
{
  BeginCall(context);

  int dummy[sizeof...(TArgs) + 1] = {0, (PushVar(context, args), 1)...};
  NotUsed(dummy);

  const auto arguments = sizeof...(TArgs);
  return CallFunction(context, arguments);
}

template <typename TReturn, typename... TArgs>
TReturn
FunctionVar::Call(Context* context, TArgs... args) const
{
  const bool ret = SubCall(context, args...);
  if(!ret)
  {
    throw std::runtime_error(CollectError(context));
  }

  const auto match = DukTemplate<TReturn>::CanMatch(context, -1, 0);

  if(match.empty())
  {
    const TReturn ret = DukTemplate<TReturn>::Parse(context, -1);
    DoneFunction(context);
    return ret;
  }

  DoneFunction(context);

  // todo: handle invalid return type
  DIE("Invalid return type");

  return Default<TReturn>();
}

template <typename... TArgs>
void
FunctionVar::VoidCall(Context* context, TArgs... args) const
{
  const bool ret = SubCall(context, args...);
  if(!ret)
  {
    throw std::runtime_error(CollectError(context));
  }
  DoneFunction(context);
}


#endif  // EUPHORIA_DUK_FUNCTION_H
