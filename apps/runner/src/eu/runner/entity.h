#pragma once
#include <algorithm>

namespace eu::runner
{
    struct Entity;
    struct Component;
    struct SpatialComponent;
    struct EntitySystem;
    struct World;

    enum class UpdateStage
    {
        start_frame,
        before_physics,
        physics,
        after_physics,
        end_frame
    };
    constexpr unsigned int update_stage_count = static_cast<unsigned int>(UpdateStage::end_frame) + 1;

    struct UpdateStageAndPrio
    {
        UpdateStage stage;
        int prio;
    };

    template<typename TSystem>
    struct SystemWithPrio
    {
        TSystem* system;
        int prio;
    };

    constexpr std::size_t C(UpdateStage s)
    {
        return static_cast<std::size_t>(s);
    }

    template<typename TSystem>
    struct UpdateHandler
    {
        std::array<std::vector<SystemWithPrio<TSystem>>, update_stage_count> stages;

        void add(TSystem* system)
        {
            const UpdateStageAndPrio s = system->get_stage();

            auto& systems = stages[C(s.stage)];

            systems.emplace_back(system, s.prio);
            std::sort(systems.begin(), systems.end(), [](const auto& lhs, const auto& rhs)
                {
                    return lhs.prio < rhs.prio;
                });
        }

        void update(UpdateStage stage, float dt)
        {
            auto& systems = stages[C(stage)];
            for (auto& sys : systems)
            {
                sys.system->update(dt);
            }
        }
    };

    struct Entity
    {
        World* world = nullptr;
        std::vector<std::unique_ptr<Component>> components;
        std::vector<std::unique_ptr<EntitySystem>> systems;
        UpdateHandler<EntitySystem> updates;
        SpatialComponent* root = nullptr;

        void add_component(std::unique_ptr<Component> c);
        void add_system(std::unique_ptr<EntitySystem> system);
    };

    struct Component
    {
        Component() = default;
        virtual ~Component() = default;

        Component(const Component& rhs) = delete;
        Component(Component&&) = delete;
        void operator=(const Component& rhs) = delete;
        void operator=(Component&&) = delete;

        virtual Hsh get_type() = 0;
    };

    struct SpatialComponent : Component
    {
        void set_transform(const m4& t);
        const m4& get_transform() const;

    private:
        m4 transform = m4_identity;
        // todo(gustav): add hierarchy
    };

    // essentially a named update
    struct EntitySystem
    {
        EntitySystem() = default;
        virtual ~EntitySystem() = default;

        EntitySystem(const EntitySystem& rhs) = delete;
        explicit EntitySystem(Component&&) = delete;
        void operator=(const EntitySystem& rhs) = delete;
        void operator=(EntitySystem&&) = delete;

        virtual UpdateStageAndPrio get_stage() = 0;

        virtual void* add_component(Component* component) = 0;

        virtual void update(float dt) = 0;
    };

    struct WorldSystem
    {
        WorldSystem() = default;
        virtual ~WorldSystem() = default;

        WorldSystem(const WorldSystem& rhs) = delete;
        explicit WorldSystem(Component&&) = delete;
        void operator=(const WorldSystem& rhs) = delete;
        void operator=(WorldSystem&&) = delete;

        virtual UpdateStageAndPrio get_stage() = 0;

        virtual void* add_component(Entity* entity, Component* component) = 0;

        virtual void update(float dt) = 0;
    };

    struct World
    {
        std::vector<std::unique_ptr<Entity>> entities;
        std::vector<std::unique_ptr<WorldSystem>> systems;
        UpdateHandler<WorldSystem> updates;

        Entity* add_entity();
        
        void add_system(std::unique_ptr<WorldSystem> sys);
        void on_add_component(Entity* entity, Component* component);

        void update(UpdateStage stage, float dt);
    };

}


/**
 * @}
*/
