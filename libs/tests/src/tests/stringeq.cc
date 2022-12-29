#include "tests/stringeq.h"

#include "catch.hpp"
#include <algorithm>

#include "core/editdistance.h"
#include "assert/assert.h"
#include "core/stringutils.h"

#include "tests/vectortostring.h"


namespace
{
    std::string escape_string(const std::string& str)
    {
        return ::Catch::Detail::stringify(str);
    }
}

namespace euphoria::tests
{
    FalseString
    string_is_equal(const std::string& lhs, const std::string& rhs)
    {
        const auto s = core::find_first_index_of_mismatch(lhs, rhs);
        ASSERTX((s==std::string::npos && lhs == rhs) || (s != std::string::npos && lhs != rhs), s, lhs, rhs);
        if(s != std::string::npos )
        {
            return FalseString::create_false
            (
                "lhs: {} and rhs: {}, lengths are {} vs {}, first diff at {} "
                "with {}/{}, edit-distance is {}"_format
                (
                    escape_string(lhs), escape_string(rhs),
                    lhs.size(), rhs.size(),
                    s,
                    core::char_to_string(lhs[s]),
                    core::char_to_string(rhs[s]),
                    core::edit_distance(lhs, rhs)
                )
            );
        }

        return FalseString::create_true();
    }


    FalseString
    string_is_equal(const std::vector<std::string>& lhs, const std::vector<std::string>& rhs)
    {
        auto size_equal = FalseString::create_true();
        if(lhs.size() != rhs.size())
        {
            size_equal = FalseString::create_false
            (
                "Size mismatch: {} vs {}: {} != {}"_format
                (
                    lhs.size(),
                    rhs.size(),
                    vector_to_string(lhs, escape_string),
                    vector_to_string(rhs, escape_string)
                )
            );
        }

        const auto size = std::min(lhs.size(), rhs.size());
        for(size_t i =0; i < size; i+=1)
        {
            const FalseString equals = string_is_equal(lhs[i], rhs[i]);
            if(!equals)
            {
                const auto first_invalid = !size_equal
                    ? "{}, and first invalid"_format(size_equal.str)
                    : "{}vs{} First invalid"_format
                    (
                        vector_to_string(lhs, escape_string),
                        vector_to_string(rhs, escape_string)
                    );

                return FalseString::create_false
                (
                    "{} value at index {}, {}"_format
                    (
                        first_invalid,
                        i,
                        equals.str
                    )
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
