#include "str.h"
#include <cassert>
#include <string>

namespace euphoria::core
{
    Str::operator std::string() const  // NOLINT
    {
        return ToString();
    }

    std::string
    Str::ToString() const
    {
        return stream_.str();
    }

}  // namespace euphoria::core
