#pragma once
#include <algorithm>
#include <set>

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
        Entity(const std::string& n, std::set<Hsh>&& t);

        Entity(const Entity&) = delete;
        Entity(Entity&&) = delete;
        void operator=(const Entity&) = delete;
        void operator=(Entity&&) = delete;

        const std::string name;

        // todo(Gustav): split HshSt into 2, one with a primary hash and one without
        World* world = nullptr;
        std::vector<std::unique_ptr<Component>> components;
        std::vector<std::unique_ptr<EntitySystem>> systems;
        UpdateHandler<EntitySystem> updates;

        void add_component(std::unique_ptr<Component> c);
        void add_system(std::unique_ptr<EntitySystem> system);
        bool has_tag(const Hsh& h) const;

        SpatialComponent* get_root() const;
        void set_root(SpatialComponent* c);

        void imgui();

    private:
        SpatialComponent* root = nullptr;
        std::set<Hsh> tags;
    };

    struct Component
    {
        Component() = default;
        virtual ~Component() = default;

        Component(const Component& rhs) = delete;
        Component(Component&&) = delete;
        void operator=(const Component& rhs) = delete;
        void operator=(Component&&) = delete;

        static const HshSt& type();
        virtual const HshSt& get_type();

        virtual const char* display() = 0;
        virtual void imgui() = 0;
    };
#define EU_DEC_COMPONENT_TYPE() static const HshSt& type(); const HshSt& get_type() override
#define EU_IMP_COMPONENT_TYPE(COMP, PAREN) \
    const eu::HshSt& COMP::type() { const static auto s = PAREN::type().combine(Hsh{#COMP}); return s; }\
    const eu::HshSt& COMP::get_type() { return type(); }

    template<typename T>
    concept ComponentLike = std::is_base_of_v<Component, T> && requires
    {
        { T::type() } -> std::same_as<const HshSt&>;
    };

    template<ComponentLike TComp> TComp* component_cast(Component* c)
    {
        if (c && c->get_type().is(TComp::type()))
        {
            return static_cast<TComp*>(c);
        }
        else
        {
            return nullptr;
        }
    }

    struct SpatialComponent : Component
    {
        void set_transform(const m4& t);
        const m4& get_transform() const;

        EU_DEC_COMPONENT_TYPE();

        void imgui() override;
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

        virtual void add_component(Component* component) = 0;
        virtual void on_root_changed(SpatialComponent* root) = 0;

        virtual void update(float dt) = 0;

        virtual const char* display() = 0;
        virtual void imgui() = 0;
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

        virtual void add_component(Entity* entity, Component* component) = 0;
        virtual void on_root_changed(Entity* entity, SpatialComponent* component) = 0;

        virtual const char* display() = 0;
        virtual void update(float dt) = 0;
        virtual void imgui() = 0;
    };

    struct World
    {
        std::vector<std::unique_ptr<Entity>> entities;
        std::vector<std::unique_ptr<WorldSystem>> systems;
        UpdateHandler<WorldSystem> updates;

        Entity* add_entity(const std::string& name, std::set<Hsh>&& tags);
        
        void add_system(std::unique_ptr<WorldSystem> sys);
        void on_add_component(Entity* entity, Component* component);
        void on_root_changed(Entity* entity, SpatialComponent* component);

        void update(UpdateStage stage, float dt);
        void gui();
    };

}


/**
 * @}
*/
