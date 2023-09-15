#pragma once

#include "core/ints.h"

namespace eu::core::ecs
{
    // using EntityId = std::uint32_t;
    // using EntityVersion = std::uint16_t;
    // using ComponentId = std::uint32_t;

    // todo(Gustav): add versioning
    enum EntityHandle : U64 {};
    enum ComponentIndex : U8 {};
}
