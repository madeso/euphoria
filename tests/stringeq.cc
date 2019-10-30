#include "tests/stringeq.h"

#include <sstream>
#include <algorithm>

#include "core/editdistance.h"
#include "core/assert.h"
#include "core/stringutils.h"

#include "catch.hpp"

namespace
{
    std::string EscapeString(const std::string& str)
    {
        return ::Catch::Detail::stringify(str);
    }

    std::string VectorToString(const std::vector<std::string>& v, bool one_line)
    {
        std::ostringstream ss;
        bool first = true;

        std::string newline = one_line ? "" : "\n";

        ss << newline << "{" << newline;
        for(const auto s: v)
        {
            if(first) first = false;
            else ss << ", " << newline;
            if(one_line == false) ss << "  ";
            ss << EscapeString(s);
        }
        if(!v.empty()) ss << newline;
        ss << "}" << newline;

        return ss.str();
    }

    std::string VectorToString(const std::vector<std::string>& v)
    {
        const auto oneline = VectorToString(v, true);
        if( oneline.size() <15)  return oneline;
        else return VectorToString(v, false);
    }
}

namespace euphoria::tests
{
    FalseString FalseString::False(const std::string& text)
    {
        return {text};
    }

    FalseString
    FalseString::True()
    {
        return {""};
    }

    FalseString::operator bool() const
    {
        return str.empty();
    }

    std::ostream&
    operator<<(std::ostream& s, const FalseString& f)
    {
        if(f)
        {
            s << "<ok>";
        }
        else
        {
            s << f.str;
        }
        return s;
    }

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
            ss << VectorToString(lhs) << " " << VectorToString(rhs);
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
                    ss << VectorToString(lhs) << "vs" << VectorToString(rhs);
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
