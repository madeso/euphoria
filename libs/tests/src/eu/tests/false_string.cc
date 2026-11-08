#include "eu/tests/false_string.h"

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
    FalseString::is_true() const
    {
        return reason.has_value() == false;
    }


    FalseString::operator bool() const
    {
        return is_true();
    }


    FalseString
    FalseString::Combine(const FalseString& lhs, const FalseString& rhs)
    {
        if(lhs.is_true() && rhs.is_true()) { return FalseString::True(); }

        if(lhs.reason.has_value() && rhs.reason.has_value())
        {
            return FalseString::False( *lhs.reason + "\n" + *rhs.reason );
        }

        if( !lhs.is_true() ) { return lhs; }
        else
        {
            assert(!rhs.is_true());
            return rhs;
        }
    }


    std::ostream&
    operator<<(std::ostream& s, const FalseString& f)
    {
        if(f.reason.has_value())
        {
            s << *f.reason; 
        }
        else
        {
            s << "<true>";
        }
        return s;
    }
}
