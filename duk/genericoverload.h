#ifndef EUPHORIA_DUK_GENERICOVERLOAD_H
#define EUPHORIA_DUK_GENERICOVERLOAD_H

#include <string>
#include <vector>
#include <utility>

#include "core/stringmerger.h"
#include "core/str.h"

#include "duk/overload.h"
#include "duk/context.h"
#include "duk/template.h"

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

#endif  // EUPHORIA_DUK_GENERICOVERLOAD_H
