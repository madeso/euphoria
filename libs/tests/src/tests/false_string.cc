#include "tests/false_string.h"

#include <cassert>

namespace eu::tests
{
    FalseString FalseString::False(const std::string& reason)
    {
        assert(!reason.empty());
        return {reason};
    }


    FalseString
    FalseString::True()
    {
        return {std::nullopt};
    }

    bool
    FalseString::IsTrue() const
    {
        return reason.has_value() == false;
    }


    FalseString::operator bool() const
    {
        return IsTrue();
    }


    FalseString
    FalseString::Combine(const FalseString& lhs, const FalseString& rhs)
    {
        if(lhs.IsTrue() && rhs.IsTrue()) { return FalseString::True(); }

        if(!lhs.IsTrue() && !rhs.IsTrue())
        {
            return FalseString::False( *lhs.reason + "\n" + *rhs.reason );
        }

        if( !lhs.IsTrue() ) { return lhs; }
        else
        {
            assert(!rhs.IsTrue());
            return rhs;
        }
    }


    std::ostream&
    operator<<(std::ostream& s, const FalseString& f)
    {
        if(f)
        {
            s << "<true>";
        }
        else
        {
            s << *f.reason;
        }
        return s;
    }
}
