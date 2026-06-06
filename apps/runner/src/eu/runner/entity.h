#pragma once

namespace eu::runner
{
    struct Entity;
    struct Component;
    struct SpatialComponent;
    struct EntitySystem;

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

    struct EntitySystemWithPrio
    {
        EntitySystem* system;
        int prio;
    };

    struct UpdateHandler
    {
        std::array<std::vector<EntitySystemWithPrio>, update_stage_count> stages;

        void add(UpdateStage stage, EntitySystem* system, int prio);
        void update(UpdateStage stage, float dt);
    };

    struct Entity
    {
        std::vector<std::unique_ptr<Component>> components;
        std::vector<std::unique_ptr<EntitySystem>> systems;
        UpdateHandler updates;
        SpatialComponent* root;

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

        virtual void load() = 0;
        virtual void unload() = 0;
        virtual void was_added_to_world() = 0;
        virtual void was_removed_from_world() = 0;
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
        EntitySystem(Component&&) = delete;
        void operator=(const EntitySystem& rhs) = delete;
        void operator=(EntitySystem&&) = delete;

        virtual UpdateStageAndPrio get_stage() = 0;

        virtual void* add_component(Component* component) = 0;
        virtual void* remove_component(Component* component);

        virtual void update(float dt) = 0;
    };

    struct World
    {
        std::vector<std::unique_ptr<Entity>> entities;
        Entity* add_entity();

        void update(UpdateStage stage, float dt);
    };

}


/**
 * @}
*/
