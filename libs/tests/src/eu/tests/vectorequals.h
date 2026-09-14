#pragma once

#include <vector>
#include <sstream>
#include <functional>
#include <algorithm>

#include "fmt/format.h"

#include "eu/tests/false_string.h"
#include "eu/tests/vectortostring.h"


namespace eu::tests
{


template
<
    typename T,
    // std::function<std::string (const T&)> 
    typename TToString,
    // std::function<FalseString (const T&, const T&)> 
    typename TCompare
>
FalseString
vector_equals
(
    const std::vector<T>& lhs,
    const std::vector<T>& rhs,
    TToString fun_to_string,
    TCompare fun_compare
)
{
    auto size_equal = FalseString::True();
    if(lhs.size() != rhs.size())
    {
        std::ostringstream ss;
        ss << "Size mismatch: "
            << lhs.size()
            << " vs "
            << rhs.size();
        ss << VectorToString(lhs, fun_to_string) << " " << VectorToString(rhs, fun_to_string);
        size_equal = FalseString::False(ss.str());
    }

    const auto smallest_size = std::min(lhs.size(), rhs.size());
    for(size_t array_index =0; array_index < smallest_size; array_index+=1)
    {
        const FalseString equals = fun_compare(lhs[array_index], rhs[array_index]);
        if(equals.reason.has_value())
        {
            std::ostringstream ss;

            if(size_equal.reason.has_value())
            {
                ss << *size_equal.reason << ", and first invalid";
            }
            else
            {
                const auto a_lhs = VectorToStringEx(lhs, fun_to_string);
                const auto a_rhs = VectorToStringEx(rhs, fun_to_string);
                const auto same = a_lhs.second == a_rhs.second;
                const auto lhs_str = same || a_lhs.second==false? a_lhs.first : VectorToString(lhs, false, fun_to_string);
                const auto rhs_str = same || a_rhs.second==false? a_rhs.first : VectorToString(rhs, false, fun_to_string);
                const auto oneliner = same && a_lhs.second == true;
                const auto vs = oneliner ? " vs " : "vs";
                if (oneliner == false)
                {
                    ss << "  ";
                }
                ss << lhs_str << vs << rhs_str;
                if (oneliner)
                {
                    ss << " ";
                }
                ss << "First invalid";
            }


            ss << " value at index ";
            ss << array_index << ", " << *equals.reason;
            return FalseString::False(ss.str());
        }
    }

    if(!size_equal)
    {
        return size_equal;
    }

    return FalseString::True();
}


template<typename T>
FalseString vector_equals
(
    const std::vector<T>& lhs,
    const std::vector<T>& rhs
)
{
    return VectorEquals(lhs, rhs,
        [](const T& t) { return fmt::format("{}", t);},
        [](const T& l, const T& r) -> FalseString
        {
            if(l != r)
            {
                return FalseString::False(fmt::format("{} != {}", l, r));
            }
            else
            {
                return FalseString::True();
            }
        }
    );
}


}
