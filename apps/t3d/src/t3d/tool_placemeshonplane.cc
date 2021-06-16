#include "t3d/tool_placemeshonplane.h"

#include "imgui/imgui.h"

#include "core/intersection.h"
#include "core/numeric.h"

#include "render/actor.h"

#include "t3d/editor.h"
#include "t3d/grid.h"
#include "t3d/tilelibrary.h"

namespace euphoria::t3d
{
    tool_place_mesh_on_a_plane::tool_place_mesh_on_a_plane(std::shared_ptr<placed_mesh> aactor)
        : actor(aactor)
        , plane(core::plane::from_normal_and_point(core::unit3f::up(), core::vec3f::zero()))
    {}


    bool
    tool_place_mesh_on_a_plane::is_busy(editor*)
    {
        return true;
    }


    float
    snap_to(float val, float step)
    {
        return core::round(val, step);
    }


    void
    tool_place_mesh_on_a_plane::step(editor* editor)
    {
        auto ray = editor->camera
                        .clip_to_world_ray(
                                editor->viewport.to_clip_coord(editor->mouse))
                        .get_normalized();

        const auto t = core::get_intersection(ray, plane);

        // LOG_INFO("Step " << editor->mouse << ", " << t);

        if(t < 0)
        {
            return;
        }
        // do intersection with plane...
        auto p = ray.get_point(t);

        if(editor->grid->snap_enabled)
        {
            p.x = snap_to(p.x, editor->grid->small_step);
            p.z = snap_to(p.z, editor->grid->small_step);
        }

        actor->actor->position = p;
    }


    void
    tool_place_mesh_on_a_plane::on_mouse(editor* editor, core::MouseButton button, bool down)
    {
        if(down)
            return;
        switch(button)
        {
        case core::MouseButton::left: editor->tools.pop_tool(); break;
        default: break;
        }
    }


    void
    tool_place_mesh_on_a_plane::on_key(editor* editor, core::key key, bool down)
    {
        if(down)
            return;
        switch(key)
        {
        case core::key::return_key: editor->tools.pop_tool(); break;
        default: break;
        }
    }


    void
    tool_place_mesh_on_a_plane::on_scroll(editor*, const core::vec2i&)
    {}



    void
    tool_place_mesh_on_a_plane::on_editor(editor* editor)
    {
        ImGui::Text("Placing object in world!");


        if(editor->selected_mesh != nullptr)
        {
            if(editor->tile_library->run_imgui_list(&editor->selected_mesh))
            {
                actor->tile = editor->selected_mesh;
                actor->actor->mesh_ = editor->selected_mesh->mesh;
            }
        }
    }
}

