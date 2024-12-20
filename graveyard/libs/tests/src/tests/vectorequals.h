#pragma once



#include <functional>
#include <algorithm>

#include "tests/falsestring.h"
#include "tests/vectortostring.h"


namespace eu::tests
{
    template
    <
        typename T,
        // std::function<std::string (const T&)>
        typename TToString,
        // std::function<false_string (const T&, const T&)>
        typename TCompare
    >
    FalseString
    is_vector_equal
    (
        const std::vector<T>& lhs,
        const std::vector<T>& rhs,
        TToString to_string_functor,
        TCompare compare_functor
    )
    {
        auto size_equal = FalseString::create_true();
        if(lhs.size() != rhs.size())
        {
            size_equal = FalseString::create_false
            (
                fmt::format
                (
                    "Size mismatch: {} vs {}{} {}",
                    lhs.size(), rhs.size(),
                    from_vector_to_string(lhs, to_string_functor),
                    from_vector_to_string(rhs, to_string_functor)
                )
            );
        }

        const auto size = std::min(lhs.size(), rhs.size());
        for(size_t index =0; index < size; index+=1)
        {
            const FalseString equals = compare_functor(lhs[index], rhs[index]);
            if(!equals)
            {
                const auto first_invalid = !size_equal
                    ? fmt::format("{}, and first invalid", size_equal.str)
                    : ([&]()
                    {
                        const auto alhs = from_vector_to_string_ex(lhs, to_string_functor);
                        const auto arhs = from_vector_to_string_ex(rhs, to_string_functor);
                        const auto same = alhs.second == arhs.second;
                        const auto lhsstr = same || alhs.second==false? alhs.first : from_vector_to_string_impl(lhs, false, to_string_functor);
                        const auto rhsstr = same || arhs.second==false? arhs.first : from_vector_to_string_impl(rhs, false, to_string_functor);
                        const auto oneliner = same && alhs.second == true;
                        
                        if(oneliner)
                        {
                            return fmt::format("{} vs {} First invalid", lhsstr, rhsstr);
                        }
                        else
                        {
                            return fmt::format("  {}vs{}First invalid", lhsstr, rhsstr);
                        }
                    })()
                    ;
                return FalseString::create_false
                (
                    fmt::format("{} value at index {}, {}", first_invalid, index, equals.str)
                );
            }
        }

        if(!size_equal)
        {
            return size_equal;
        }

        return FalseString::create_true();
    }
}
