#ifndef EUPHORIA_ENTITYID_H
#define EUPHORIA_ENTITYID_H

#include <cstdint>

namespace euphoria::core::ecs
{
    using EntityId      = std::uint32_t;
    using EntityVersion = std::uint16_t;

    using ComponentId = std::uint32_t;

}  // namespace euphoria::core

#endif  // EUPHORIA_ENTITYID_H
