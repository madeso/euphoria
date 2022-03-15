#include "t3d/editor.h"


#include "core/intersection.h"

#include "render/actor.h"

#include "window/imguizmo.h"

#include "t3d/tool.h"
#include "t3d/tilelibrary.h"
#include "t3d/grid.h"


namespace euphoria::t3d
{
    Editor::Editor
    (
        t3d::Grid* agrid,
        render::World* aworld,
        t3d::TileLibrary* atile_library
    )
        : grid(agrid)
        , world(aworld)
        , tile_library(atile_library)
        , camera(core::Mat4f::identity(), core::Mat4f::identity())
        , viewport(core::Recti::from_width_height(10, 10))
        , mouse(core::Vec2i{ 0, 0 })
    {
    }


    std::shared_ptr<PlacedMesh>
    Editor::get_first_selected_or_null()
    {
        for (auto mesh : placed_meshes)
        {
            if (mesh->is_selected)
            {
                return mesh;
            }
        }

        return nullptr;
    }


    void
    Editor::set_all_selected(bool is_selected)
    {
        for (auto mesh : placed_meshes)
        {
            mesh->is_selected = is_selected;
        }
    }


    std::vector<std::shared_ptr<PlacedMesh>>
    Editor::raycast(const core::UnitRay3f& ray)
    {
        std::vector<std::shared_ptr<PlacedMesh>> r;

        for (auto mesh : placed_meshes)
        {
            const auto transformed_ray = ray.get_transform(mesh->actor->calculate_model_matrix().get_inverted());
            const auto collision = core::get_intersection(transformed_ray, mesh->tile->aabb);

            if (collision.intersected)
            {
                const auto mesh_collision = core::get_intersection(transformed_ray, mesh->tile->collison);

                if(mesh_collision)
                {
                    r.emplace_back(mesh);
                }
            }
        }

        return r;
    }


    // current tool callbacks
    bool
    Editor::is_busy()
    {
        return tools.get_current_tool()->is_busy(this);
    }


    void
    Editor::step()
    {
        tools.get_current_tool()->step(this);
    }


    void
    Editor::on_mouse(core::MouseButton button, const KeyboardState& state, bool down)
    {
        tools.get_current_tool()->on_mouse(this, button, state, down);
    }


    void
    Editor::on_key(core::Key key, bool down)
    {
        tools.get_current_tool()->on_key(this, key, down);
    }


    void
     Editor::on_scroll(const core::Vec2i& scroll)
    {
        tools.get_current_tool()->on_scroll(this, scroll);
    }


    void
    Editor::on_editor()
    {
        tools.get_current_tool()->on_editor(this);
    }


    std::vector<int>
    Editor::get_selected_indices() const
    {
        std::vector<int> selections;

        int index = 0;
        for (auto mesh : placed_meshes)
        {
            if (mesh->is_selected)
            {
                selections.emplace_back(index);
            }

            index += 1;
        }

        return selections;
    }


    namespace
    {
        std::optional<core::Vec3f>
        get_position_snap(const Grid& grid)
        {
            if (grid.snap_enabled)
            {
                return core::Vec3f{ grid.small_step, grid.small_step , grid.small_step };
            }
            else
            {
                return std::nullopt;
            }
        }


        std::optional<core::Angle>
        get_angle_snap(const Grid& grid)
        {
            if (grid.snap_enabled)
            {
                return grid.angle_snap;
            }
            else
            {
                return std::nullopt;
            }
        }


        void
        run_single_selection
        (
            const Grid& grid, std::shared_ptr<PlacedMesh> mesh,
            bool is_translate, bool global_space, const core::CompiledCamera3& cc,
            bool translate_x, bool translate_y, bool translate_z,
            bool rotate_x, bool rotate_y, bool rotate_z
        )
        {
            if (is_translate)
            {
                window::imgui::guizmo::translate
                (
                    !global_space,
                    get_position_snap(grid),
                    cc.view,
                    cc.projection,
                    mesh->actor->calculate_model_matrix(),
                    translate_x, translate_y, translate_z,
                    &mesh->actor->position
                );
            }
            else
            {
                window::imgui::guizmo::rotate
                (
                    !global_space,
                    get_angle_snap(grid),
                    cc.view,
                    cc.projection,
                    mesh->actor->calculate_model_matrix(),
                    rotate_x, rotate_y, rotate_z,
                    &mesh->actor->rotation
                );
            }
        }


        core::Vec3f
        get_common_position
        (
            const std::vector<std::shared_ptr<PlacedMesh>>& meshes,
            const std::vector<int>& selections
        )
        {
            ASSERT(selections.empty() == false);

            auto p = core::Vec3f{ 0.0f, 0.0f, 0.0f };
            int count = 0;

            for (auto selection : selections)
            {
                p += meshes[selection]->actor->position;
                count += 1;
            }

            return p / static_cast<float>(count);
        }


        core::Quatf
        get_common_rotation
        (
            const std::vector<std::shared_ptr<PlacedMesh>>& meshes,
            const std::vector<int>& selections
        )
        {
            ASSERT(selections.empty() == false);

            // todo(Gutav): is this the correct way?
            // should we calculate the average rotation instead of just grabbing one?
            return meshes[selections[0]]->actor->rotation;
        }


        core::Vec3f
        snap_or_not
        (
            std::optional<core::Vec3f> snap,
            const core::Vec3f& point
        )
        {
            if (snap.has_value())
            {
                return core::Vec3f
                {
                    snap_to(point.x, snap->x),
                    snap_to(point.y, snap->y),
                    snap_to(point.z, snap->z)
                };
            }
            else
            {
                return point;
            }
        }


        void
        run_multi_selection_translate
        (
            const Grid& grid,
            std::vector<std::shared_ptr<PlacedMesh>>* meshes_ptr, const std::vector<int>& selections,
            bool global_space, const core::CompiledCamera3& cc,
            bool translate_x, bool translate_y, bool translate_z
        )
        {
            auto& meshes = *meshes_ptr;
            const auto original_base_position = get_common_position(meshes, selections);
            const auto original_position = snap_or_not(get_position_snap(grid), original_base_position);
            const auto rotation = get_common_rotation(meshes, selections);

            auto position = original_position;

            const auto has_been_translated = window::imgui::guizmo::translate
            (
                !global_space,
                get_position_snap(grid),
                cc.view,
                cc.projection,
                render::calculate_model_matrix(position, rotation),
                translate_x, translate_y, translate_z,
                &position
            );

            if (has_been_translated)
            {
                const auto translation = core::Vec3f::from_to(original_position, position);

                for (auto selection : selections)
                {
                    meshes[selection]->actor->position += translation;
                }
            }
        }

        void
        run_multi_selection_rotate
        (
            const Grid& grid,
            std::vector<std::shared_ptr<PlacedMesh>>* meshes_ptr, const std::vector<int>& selections,
            bool global_space, const core::CompiledCamera3& cc,
            bool rotate_x, bool rotate_y, bool rotate_z
        )
        {
            auto& meshes = *meshes_ptr;
            const auto position = get_common_position(meshes, selections);
            const auto original_rotation = get_common_rotation(meshes, selections);

            auto rotation = original_rotation;

            const auto has_been_rotated = window::imgui::guizmo::rotate
            (
                !global_space,
                get_angle_snap(grid),
                cc.view,
                cc.projection,
                render::calculate_model_matrix(position, rotation),
                rotate_x, rotate_y, rotate_z,
                &rotation
            );

            if (has_been_rotated)
            {
                const auto change = core::Quatf::from_to(original_rotation, rotation);

                for (auto selection : selections)
                {
                    auto act = meshes[selection]->actor;
                    act->rotation = (act->rotation * change).get_normalized();
                }
            }
        }

        void
        run_multi_selection
        (
            const Grid& grid,
            std::vector<std::shared_ptr<PlacedMesh>>* meshes_ptr, const std::vector<int>& selections,
            bool is_translate, bool global_space, const core::CompiledCamera3& cc,
            bool translate_x, bool translate_y, bool translate_z,
            bool rotate_x, bool rotate_y, bool rotate_z
        )
        {
            if (is_translate)
            {
                run_multi_selection_translate
                (
                    grid,
                    meshes_ptr, selections,
                    global_space, cc,
                    translate_x, translate_y, translate_z
                );
            }
            else
            {
                run_multi_selection_rotate
                (
                    grid,
                    meshes_ptr, selections,
                    global_space, cc,
                    rotate_x, rotate_y, rotate_z
                );
            }
        }
    }


    void
    Editor::run_tools
    (
        bool is_translate, bool global_space, const core::CompiledCamera3& cc,
        bool translate_x, bool translate_y, bool translate_z,
        bool rotate_x, bool rotate_y, bool rotate_z
    )
    {
        const auto selections = get_selected_indices();
        if (selections.empty()) { return; }

        if (selections.size() == 1)
        {
            auto mesh = placed_meshes[selections[0]];
            run_single_selection
            (
                *grid, placed_meshes[selections[0]],
                is_translate, global_space, cc,
                translate_x, translate_y, translate_z,
                rotate_x, rotate_y, rotate_z
            );
        }
        else
        {
            run_multi_selection
            (
                *grid, &placed_meshes, selections,
                is_translate, global_space, cc,
                translate_x, translate_y, translate_z,
                rotate_x, rotate_y, rotate_z
            );
        }
    }
}
