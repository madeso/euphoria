#pragma once



#include <functional>
#include <algorithm>

#include "tests/falsestring.h"
#include "tests/vectortostring.h"


namespace euphoria::tests
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
    vector_is_equal
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
                "Size mismatch: {} vs {}{} {}"_format
                (
                    lhs.size(), rhs.size(),
                    vector_to_string(lhs, to_string_functor),
                    vector_to_string(rhs, to_string_functor)
                )
            );
        }

        const auto size = std::min(lhs.size(), rhs.size());
        for(size_t i =0; i < size; i+=1)
        {
            const FalseString equals = compare_functor(lhs[i], rhs[i]);
            if(!equals)
            {
                const auto first_invalid = !size_equal
                    ? "{}, and first invalid"_format(size_equal.str)
                    : ([&]()
                    {
                        const auto alhs = vector_to_string_ex(lhs, to_string_functor);
                        const auto arhs = vector_to_string_ex(rhs, to_string_functor);
                        const auto same = alhs.second == arhs.second;
                        const auto lhsstr = same || alhs.second==false? alhs.first : vector_to_string_impl(lhs, false, to_string_functor);
                        const auto rhsstr = same || arhs.second==false? arhs.first : vector_to_string_impl(rhs, false, to_string_functor);
                        const auto oneliner = same && alhs.second == true;
                        
                        if(oneliner)
                        {
                            return "{} vs {} First invalid"_format(lhsstr, rhsstr);
                        }
                        else
                        {
                            return "  {}vs{}First invalid"_format(lhsstr, rhsstr);
                        }
                    })()
                    ;
                return FalseString::create_false
                (
                    "{} value at index {}, {}"_format(first_invalid, i, equals.str)
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
