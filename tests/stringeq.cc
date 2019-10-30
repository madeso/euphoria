#include "tests/stringeq.h"

#include <sstream>
#include <algorithm>

#include "core/editdistance.h"
#include "core/assert.h"
#include "core/stringutils.h"


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
        ASSERTX((s==-1 && lhs == rhs) || (s >= 0 && lhs != rhs), s, lhs, rhs);
        if(s >= 0 )
        {
            std::ostringstream ss;
                
            ss << "lhs: '" << lhs << "' and "
                << "rhs: '" << rhs << "'";
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
        if(lhs.size() != rhs.size())
        {
            return FalseString::False("size missmach");
        }

        for(size_t i =0; i < lhs.size(); i+=1)
        {
            const FalseString equals = StringEq(lhs[i], rhs[i]);
            if(!equals)
            {
                std::ostringstream ss;
                ss << "Invalid at index " << i << ", "
                    << equals.str;
                return FalseString::False(ss.str());
            }
        }

        return FalseString::True();
    }
}  // namespace euphoria::tests
