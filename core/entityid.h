#ifndef EUPHORIA_ENTITYID_H
#define EUPHORIA_ENTITYID_H

#include <entt/entity/entt_traits.hpp>

using Entity       = std::uint32_t;
using EntityTraits = entt::entt_traits<Entity>;
using EntityId     = typename EntityTraits::entity_type;

#endif  // EUPHORIA_ENTITYID_H
