#pragma once

#include <vector>
#include <sstream>
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
    false_string
    vector_is_equal
    (
        const std::vector<T>& lhs,
        const std::vector<T>& rhs,
        TToString ToString,
        TCompare Compare
    )
    {
        auto size_equal = false_string::create_true();
        if(lhs.size() != rhs.size())
        {
            std::ostringstream ss;
            ss << "Size mismatch: "
                << lhs.size()
                << " vs "
                << rhs.size();
            ss << vector_to_string(lhs, ToString) << " " << vector_to_string(rhs, ToString);
            size_equal = false_string::create_false(ss.str());
        }

        const auto size = std::min(lhs.size(), rhs.size());
        for(size_t i =0; i < size; i+=1)
        {
            const false_string equals = Compare(lhs[i], rhs[i]);
            if(!equals)
            {
                std::ostringstream ss;

                if(!size_equal)
                {
                    ss << size_equal.str << ", and first invalid";
                }
                else
                {
                    const auto alhs = vector_to_string_ex(lhs, ToString);
                    const auto arhs = vector_to_string_ex(rhs, ToString);
                    const auto same = alhs.second == arhs.second;
                    const auto lhsstr = same || alhs.second==false? alhs.first : vector_to_string_impl(lhs, false,
                                                                                                       ToString);
                    const auto rhsstr = same || arhs.second==false? arhs.first : vector_to_string_impl(rhs, false,
                                                                                                       ToString);
                    const auto oneliner = same && alhs.second == true;
                    const auto vs = oneliner ? " vs " : "vs";
                    if(oneliner == false) ss << "  ";
                    ss << lhsstr << vs << rhsstr;
                    if(oneliner) ss << " ";
                    ss << "First invalid";
                }


                ss << " value at index ";
                ss << i << ", "
                    << equals.str;
                return false_string::create_false(ss.str());
            }
        }

        if(!size_equal)
        {
            return size_equal;
        }

        return false_string::create_true();
    }
}
