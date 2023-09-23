#include "tests/stringeq.h"

#include "catch.hpp"
#include <algorithm>

#include "base/editdistance.h"
#include "assert/assert.h"
#include "base/stringutils.h"

#include "tests/vectortostring.h"


namespace
{
    std::string escape_string(const std::string& str)
    {
        return ::Catch::Detail::stringify(str);
    }
}

namespace eu::tests
{
    FalseString
    is_string_equal(const std::string& lhs, const std::string& rhs)
    {
        const auto s = find_first_index_of_mismatch(lhs, rhs);
        ASSERTX((s==std::string::npos && lhs == rhs) || (s != std::string::npos && lhs != rhs), s, lhs, rhs);
        if(s != std::string::npos )
        {
            return FalseString::create_false
            (
                fmt::format
                (
                    "lhs: {} and rhs: {}, lengths are {} vs {}, first diff at {} "
                    "with {}/{}, edit-distance is {}",
                    escape_string(lhs), escape_string(rhs),
                    lhs.size(), rhs.size(),
                    s,
                    from_char_to_string(lhs[s]),
                    from_char_to_string(rhs[s]),
                    calc_edit_distance(lhs, rhs)
                )
            );
        }

        return FalseString::create_true();
    }


    FalseString
    is_string_equal(const std::vector<std::string>& lhs, const std::vector<std::string>& rhs)
    {
        auto size_equal = FalseString::create_true();
        if(lhs.size() != rhs.size())
        {
            size_equal = FalseString::create_false
            (
                fmt::format
                (
                    "Size mismatch: {} vs {}: {} != {}",
                    lhs.size(),
                    rhs.size(),
                    from_vector_to_string(lhs, escape_string),
                    from_vector_to_string(rhs, escape_string)
                )
            );
        }

        const auto size = std::min(lhs.size(), rhs.size());
        for(size_t index =0; index < size; index+=1)
        {
            const FalseString equals = is_string_equal(lhs[index], rhs[index]);
            if(!equals)
            {
                const auto first_invalid = !size_equal
                    ? fmt::format("{}, and first invalid", size_equal.str)
                    : fmt::format("{}vs{} First invalid",
                        from_vector_to_string(lhs, escape_string),
                        from_vector_to_string(rhs, escape_string)
                    );

                return FalseString::create_false
                (
                    fmt::format
                    (
                        "{} value at index {}, {}", 
                        first_invalid,
                        index,
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
