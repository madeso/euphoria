#pragma once

#include <cstdint>

namespace euphoria::core::ecs
{
    // todo(Gustav): change to enums
    using entity_id      = std::uint32_t;
    using entity_version = std::uint16_t;
    using component_id = std::uint32_t;
}
