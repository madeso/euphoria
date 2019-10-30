#include "tests/stringeq.h"

#include <sstream>
#include <algorithm>

#include "core/editdistance.h"
#include "core/assert.h"
#include "core/stringutils.h"


namespace euphoria::tests
{
    FalseString FalseString::False(const std::string& text) { return {text};}

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
    StringEq(const std::vector<std::string> lhs, const std::vector<std::string> rhs)
    {
        if(lhs.size() != rhs.size())
        {
            return FalseString::False("size missmach");
        }

        for(size_t i =0; i < lhs.size(); i+=1)
        {
            const auto s = core::FindFirstIndexOfMismatch(lhs[i], rhs[i]);
            ASSERTX((s==-1 && lhs[i] == rhs [i]) || (s >= 0 && lhs[i] != rhs[i]), s, lhs[i], rhs[i]);
            if(s >= 0 )
            {
                std::ostringstream ss;
                
                ss << "Invalid at index " << i << ", "
                   << "lhs: '" << lhs[i] << "' and "
                   << "rhs: '" << rhs[i] << "'";
                ss << ", lengths are "
                   << lhs[i].size()
                   << " vs "
                   << rhs[i].size();
                ss << ", first diff at " << s << " with "
                   << core::CharToString(lhs[i][s])
                   << "/"
                   << core::CharToString(rhs[i][s]);
                ss << ", edit-distance is " << core::EditDistance(lhs[i], rhs[i]);

                return FalseString::False(ss.str());
            }
        }

        return FalseString::True();
    }
}  // namespace euphoria::tests
