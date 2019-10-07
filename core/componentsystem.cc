#include "core/componentsystem.h"


namespace euphoria::core::ecs
{
    ComponentSystem::ComponentSystem(const std::string& the_name)
        : name(the_name)
    {}

    void
    ComponentSystemStore::Add(std::shared_ptr<ComponentSystem> system)
    {
        systems.emplace_back(system);
    }

    void
    ComponentSystemUpdateStore::Update(EntReg* reg, float dt) const
    {
        for (const auto s: systems)
        {
            s->Update(reg, dt);
        }
    }

    void
    ComponentSystemInitStore::OnAdd(EntityId ent) const
    {
        for (const auto s: systems)
        {
            s->OnAdd(ent);
        }
    }

    void
    ComponentSystemSpriteDrawStore::Draw(
            EntReg*                 reg,
            render::SpriteRenderer* renderer) const
    {
        for (const auto s: systems)
        {
            s->Draw(reg, renderer);
        }
    }

    void
    Systems::AddAndRegister(std::shared_ptr<ComponentSystem> system)
    {
        store.Add(system);
        system->RegisterCallbacks(this);
    }

    World::World(Systems* sys) : systems(sys)
    {
        reg.AddCallback(MakeRegistryEntityCallback(
                [this](EntityId id) { systems->init.OnAdd(id); }));
    }

    void
    World::Update(float dt)
    {
        systems->update.Update(&reg, dt);
    }

    void
    World::Draw(render::SpriteRenderer* renderer)
    {
        systems->spriteDraw.Draw(&reg, renderer);
    }

}  // namespace euphoria::core
