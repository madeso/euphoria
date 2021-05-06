#include "str.h"
#include <string>

namespace euphoria::core
{
    Str::operator std::string() const  // NOLINT
    {
        return to_string();
    }

    std::string
    Str::to_string() const
    {
        return stream_.str();
    }

}  // namespace euphoria::core

