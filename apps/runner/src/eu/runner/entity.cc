#include "eu/runner/entity.h"

#include <algorithm>

namespace eu::runner
{

    // ------------------------------------------------------------------------
	//  UpdateStageAndPrio

    // ------------------------------------------------------------------------
	//  EntitySystemWithPrio

    // ------------------------------------------------------------------------
	//  UpdateHandler

    constexpr std::size_t C(UpdateStage s)
    {
        return static_cast<std::size_t>(s);
    }

    void UpdateHandler::add(UpdateStage stage, EntitySystem* system, int prio)
    {
        auto& systems = stages[C(stage)];

        systems.emplace_back(system, prio);
        std::sort(systems.begin(), systems.end(), [](const auto& lhs, const auto& rhs)
        {
            return lhs.prio < rhs.prio;
        });
    }

    void UpdateHandler::update(UpdateStage stage, float dt)
    {
        auto& systems = stages[C(stage)];
        for (auto& sys: systems)
        {
            sys.system->update(dt);
        }
    }

    // ------------------------------------------------------------------------
	//  Entity

    void Entity::add_component(std::unique_ptr<Component> c)
    {
        components.emplace_back(std::move(c));
        Component* nc = components.back().get();
        for(auto& sys: systems)
        {
            sys->add_component(nc);
        }
    }
    void Entity::add_system(std::unique_ptr<EntitySystem> system)
    {
        systems.emplace_back(std::move(system));
        EntitySystem* ne = systems.back().get();
        const auto s = ne->get_stage();
        updates.add(s.stage, ne, s.prio);
        for (auto& c: components)
        {
            ne->add_component(c.get());
        }
    }

    // ------------------------------------------------------------------------
	//  Component

    // ------------------------------------------------------------------------
	//  SpatialComponent

    void SpatialComponent::set_transform(const m4& t)
    {
        transform = t;
    }
    const m4& SpatialComponent::get_transform() const
    {
        return transform;
    }

    // ------------------------------------------------------------------------
	//  EntitySystem


    // ------------------------------------------------------------------------
    // EntitySystemUpdateStageList


}
