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
        , mouse(core::Vec2i {0, 0})
    {
    }


    std::shared_ptr<PlacedMesh>
    Editor::get_first_selected_or_null()
    {
        for(auto mesh : placed_meshes)
        {
            if(mesh->is_selected)
            {
                return mesh;
            }
        }

        return nullptr;
    }


    void
    Editor::set_all_selected(bool is_selected)
    {
        for(auto mesh: placed_meshes)
        {
            mesh->is_selected = is_selected;
        }
    }


    std::vector<std::shared_ptr<PlacedMesh>>
    Editor::raycast(const core::UnitRay3f& ray)
    {
        std::vector<std::shared_ptr<PlacedMesh>> r;

        for(auto mesh: placed_meshes)
        {
            const auto collision = core::get_intersection(ray, mesh->tile->aabb.offset_copy(mesh->actor->position));
            
            if(collision.intersected)
            {
                r.emplace_back(mesh);
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
    Editor::on_mouse(core::MouseButton button, bool down)
    {
        tools.get_current_tool()->on_mouse(this, button, down);
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


    std::optional<core::Vec3f> get_position_snap(const Grid& grid)
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

    std::optional<core::Angle> get_angle_snap(const Grid& grid)
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
    Editor::run_tools(bool is_transform, bool global_space, const core::CompiledCamera3& cc)
    {
        const auto selections = get_selected_indices();
        if (selections.empty()) { return; }

        if (selections.size() == 1)
        {
            auto mesh = placed_meshes[selections[0]];

            const bool is_local = !global_space;

            if (is_transform)
            {
                window::imgui::guizmo::transform
                (
                    is_local,
                    get_position_snap(*grid),
                    cc.view,
                    cc.projection,
                    mesh->actor->calculate_model_matrix(),
                    &mesh->actor->position
                );
            }
            else
            {
                window::imgui::guizmo::rotate
                (
                    is_local,
                    get_angle_snap(*grid),
                    cc.view,
                    cc.projection,
                    mesh->actor->calculate_model_matrix(),
                    &mesh->actor->rotation
                );
            }
        }
    }
}
