#include "str.h"
#include <string>

namespace euphoria::core
{
    StringBuilder::operator std::string() const // NOLINT
    {
        return to_string();
    }

    std::string
    StringBuilder::to_string() const
    {
        return stream.str();
    }

}

