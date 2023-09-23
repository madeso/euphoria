#pragma once

#include "io/json.h"

namespace eu::files::enums
{
    struct Root
    {
        std::vector<std::string> names;
    };

    JSON_PARSE_FUNC(Root);
}
