#include "str.h"
#include <string>

namespace euphoria::core
{
    string_builder::operator std::string() const // NOLINT
    {
        return to_string();
    }

    std::string
    string_builder::to_string() const
    {
        return stream_.str();
    }

} // namespace euphoria::core

