#ifndef EUPHORIA_DUK_GENERICOVERLOAD_H
#define EUPHORIA_DUK_GENERICOVERLOAD_H

#include <string>
#include <vector>
#include <utility>
#include <algorithm>

#include "core/stringmerger.h"
#include "core/str.h"
#include "core/range.h"

#include "duk/function.h"
#include "duk/context.h"
#include "duk/stackparser.h"

namespace euphoria::duk
{
  template <typename Callback, typename... TArgs>
  class GenericFunction : public Function
  {
   public:
    Callback callback;

    explicit GenericFunction(Callback c)
        : callback(c)
    {
      // todo: make sure non-required are only defined at the end
    }

    template <std::size_t... I>
    std::string
    MatchesImpl(Context* ctx, std::index_sequence<I...>)
    {
      const size_t passed_argument_count = ctx->GetNumberOfArguments();
      const auto required =
          std::vector<bool>{StackParser<TArgs>::IsRequired()...};
      const std::size_t argument_count =
          std::count(required.begin(), required.end(), true);
      const std::size_t max_argument_count = sizeof...(TArgs);
      if(!IsWithin(core::MakeRange(argument_count, max_argument_count), passed_argument_count))
      {
        // todo: smaller error message when both max and min are the same
        return core::Str{} << "expected " << argument_count << " to "
                     << max_argument_count << " argument(s) but got "
                     << passed_argument_count << ".";
      }

      const std::vector<std::string> matches{StackParser<TArgs>::CanMatch(
          ctx,
          -passed_argument_count + static_cast<int>(I),
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
      const auto argument_count = ctx->GetNumberOfArguments();
      return callback(
          ctx,
          StackParser<TArgs>::Parse(
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
      core::NotUsed(ctx);
      const std::vector<std::string> type_names = {
          StackParser<TArgs>::Name(ctx)...};
      return core::StringMerger::FunctionCall().Generate(type_names);
    }
  };
}

#endif  // EUPHORIA_DUK_GENERICOVERLOAD_H
