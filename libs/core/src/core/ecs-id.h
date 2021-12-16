#pragma once

#include <cstdint>

namespace euphoria::core::ecs
{
    // todo(Gustav): change to enums
    using EntityId = std::uint32_t;
    using EntityVersion = std::uint16_t;
    using ComponentId = std::uint32_t;
}
