#include "stringeq.h"

#include <sstream>
#include <algorithm>

#include "core/editdistance.h"
#include "core/assert.h"
#include "core/stringutils.h"
#include "tests/vectortostring.h"

#include "catch.hpp"

namespace
{
    std::string EscapeString(const std::string& str)
    {
        return ::Catch::Detail::stringify(str);
    }
}

namespace euphoria::tests
{
    FalseString
    StringEq(const std::string& lhs, const std::string& rhs)
    {
        const auto s = core::FindFirstIndexOfMismatch(lhs, rhs);
        ASSERTX((s==std::string::npos && lhs == rhs) || (s != std::string::npos && lhs != rhs), s, lhs, rhs);
        if(s != std::string::npos )
        {
            std::ostringstream ss;
                
            ss << "lhs: " << EscapeString(lhs) << " and "
               << "rhs: " << EscapeString(rhs) ;
            ss << ", lengths are "
                << lhs.size()
                << " vs "
                << rhs.size();
            ss << ", first diff at " << s << " with "
                << core::CharToString(lhs[s])
                << "/"
                << core::CharToString(rhs[s]);
            ss << ", edit-distance is " << core::EditDistance(lhs, rhs);

            return FalseString::False(ss.str());
        }

        return FalseString::True();
    }


    FalseString
    StringEq(const std::vector<std::string> lhs, const std::vector<std::string> rhs)
    {
        auto size_equal = FalseString::True();
        if(lhs.size() != rhs.size())
        {
            std::ostringstream ss;
            ss << "Size mismatch: "
               << lhs.size()
               << " vs "
               << rhs.size();
            ss << VectorToString(lhs, EscapeString) << " " << VectorToString(rhs, EscapeString);
            size_equal = FalseString::False(ss.str());
        }

        const auto size = std::min(lhs.size(), rhs.size());
        for(size_t i =0; i < size; i+=1)
        {
            const FalseString equals = StringEq(lhs[i], rhs[i]);
            if(!equals)
            {
                std::ostringstream ss;

                if(!size_equal)
                {
                    ss << size_equal.str << ", and first invalid";
                }
                else
                {
                    ss << VectorToString(lhs, EscapeString) << "vs" << VectorToString(rhs, EscapeString);
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
}  // namespace euphoria::tests
