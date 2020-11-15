#ifndef EUPHORIA_CORE_CLI_POSITION_H
#define EUPHORIA_CORE_CLI_POSITION_H

#include "core/vec2.h"

#include <optional>

namespace euphoria::core
{
    std::optional<vec2i>
    GetConsolePosition();

    bool
    SetConsolePosition(const vec2i& pos);
}

#endif  // EUPHORIA_CORE_CLI_POSITION_H
