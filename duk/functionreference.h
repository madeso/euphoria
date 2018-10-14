#ifndef EUPHORIA_DUK_FUNCTION_H
#define EUPHORIA_DUK_FUNCTION_H

#include <string>
#include <vector>

#include "core/cpp.h"
#include "core/str.h"

#include "duk/stackparser.h"
#include "duk/pushvar.h"

namespace duk
{
  class Context;

  // todo: rename to DukFunc
  class FunctionReference
  {
   public:
    explicit FunctionReference(void* ptr = nullptr);

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
  struct StackParser<FunctionReference>
  {
    static bool
    IsRequired()
    {
      return true;
    }

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

    static FunctionReference
    Parse(Context* ctx, int index)
    {
      ASSERT(ctx->IsObject(index));
      return FunctionReference{ctx->GetFunctionPtr(index)};
    }

    static std::string
    Name(Context*)
    {
      return "function";
    }
  };


  template <typename... TArgs>
  bool
  FunctionReference::SubCall(Context* context, TArgs... args) const
  {
    BeginCall(context);

    int dummy[sizeof...(TArgs) + 1] = {0, (PushVar(context, args), 1)...};
    NotUsed(dummy);

    const auto arguments = sizeof...(TArgs);
    return CallFunction(context, arguments);
  }

  template <typename TReturn, typename... TArgs>
  TReturn
  FunctionReference::Call(Context* context, TArgs... args) const
  {
    const bool ret = SubCall(context, args...);
    if(!ret)
    {
      throw std::runtime_error(CollectError(context));
    }

    const auto match = StackParser<TReturn>::CanMatch(context, -1, 0);

    if(match.empty())
    {
      const TReturn ret = StackParser<TReturn>::Parse(context, -1);
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
  FunctionReference::VoidCall(Context* context, TArgs... args) const
  {
    const bool ret = SubCall(context, args...);
    if(!ret)
    {
      throw std::runtime_error(CollectError(context));
    }
    DoneFunction(context);
  }
}

#endif  // EUPHORIA_DUK_FUNCTION_H
