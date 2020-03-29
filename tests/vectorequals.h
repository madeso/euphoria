#ifndef EUPHORIA_TESTS_VECTOREQUALS_H
#define EUPHORIA_TESTS_VECTOREQUALS_H

#include <vector>
#include <sstream>
#include <functional>

#include "tests/falsestring.h"
#include "tests/vectortostring.h"


namespace euphoria::tests
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
    VectorEquals
    (
        const std::vector<T>& lhs,
        const std::vector<T>& rhs,
        TToString ToString,
        TCompare Compare
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
            ss << VectorToString(lhs, ToString) << " " << VectorToString(rhs, ToString);
            size_equal = FalseString::False(ss.str());
        }

        const auto size = std::min(lhs.size(), rhs.size());
        for(size_t i =0; i < size; i+=1)
        {
            const FalseString equals = Compare(lhs[i], rhs[i]);
            if(!equals)
            {
                std::ostringstream ss;

                if(!size_equal)
                {
                    ss << size_equal.str << ", and first invalid";
                }
                else
                {
                    const auto alhs = VectorToStringEx(lhs, ToString);
                    const auto arhs = VectorToStringEx(rhs, ToString);
                    const auto same = alhs.second == arhs.second;
                    const auto lhsstr = same || alhs.second==false? alhs.first : VectorToString(lhs, false, ToString);
                    const auto rhsstr = same || arhs.second==false? arhs.first : VectorToString(rhs, false, ToString);
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
                return FalseString::False(ss.str());
            }
        }

        if(!size_equal)
        {
            return size_equal;
        }

        return FalseString::True();
    }
    }

#endif  // EUPHORIA_TESTS_VECTOREQUALS_H

