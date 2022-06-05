#include "tests/stringeq.h"

#include "core/editdistance.h"
#include "assert/assert.h"
#include "core/stringutils.h"

#include "tests/vectortostring.h"

#include "catch.hpp"


#include <algorithm>


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
            std::ostringstream ss;

            ss << "lhs: " << escape_string(lhs) << " and "
               << "rhs: " << escape_string(rhs) ;
            ss << ", lengths are "
                << lhs.size()
                << " vs "
                << rhs.size();
            ss << ", first diff at " << s << " with "
                << core::char_to_string(lhs[s])
                << "/"
                << core::char_to_string(rhs[s]);
            ss << ", edit-distance is " << core::edit_distance(lhs, rhs);

            return FalseString::create_false(ss.str());
        }

        return FalseString::create_true();
    }


    FalseString
    string_is_equal(const std::vector<std::string>& lhs, const std::vector<std::string>& rhs)
    {
        auto size_equal = FalseString::create_true();
        if(lhs.size() != rhs.size())
        {
            std::ostringstream ss;
            ss << "Size mismatch: "
               << lhs.size()
               << " vs "
               << rhs.size();
            ss << vector_to_string(lhs, escape_string) << " " << vector_to_string(rhs, escape_string);
            size_equal = FalseString::create_false(ss.str());
        }

        const auto size = std::min(lhs.size(), rhs.size());
        for(size_t i =0; i < size; i+=1)
        {
            const FalseString equals = string_is_equal(lhs[i], rhs[i]);
            if(!equals)
            {
                std::ostringstream ss;

                if(!size_equal)
                {
                    ss << size_equal.str << ", and first invalid";
                }
                else
                {
                    ss << vector_to_string(lhs, escape_string) << "vs" << vector_to_string(rhs, escape_string);
                    ss << "First invalid";
                }


                ss << " value at index ";
                ss << i << ", "
                    << equals.str;
                return FalseString::create_false(ss.str());
            }
        }

        if(!size_equal)
        {
            return size_equal;
        }

        return FalseString::create_true();
    }
}
