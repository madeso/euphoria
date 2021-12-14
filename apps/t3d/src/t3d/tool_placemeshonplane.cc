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
    ToolPlaceMeshOnAPlane::ToolPlaceMeshOnAPlane(std::shared_ptr<PlacedMesh> aactor)
        : actor(aactor)
        , plane(core::Plane::from_normal_and_point(core::Unit3f::up(), core::Vec3f::zero()))
    {}


    bool
    ToolPlaceMeshOnAPlane::is_busy(Editor*)
    {
        return true;
    }


    float
    snap_to(float val, float step)
    {
        return core::round(val, step);
    }


    void
    ToolPlaceMeshOnAPlane::step(Editor* editor)
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
    ToolPlaceMeshOnAPlane::on_mouse(Editor* editor, core::MouseButton button, bool down)
    {
        if(down) { return; }
        switch(button)
        {
        case core::MouseButton::left: editor->tools.pop_tool(); break;
        default: break;
        }
    }


    void
    ToolPlaceMeshOnAPlane::on_key(Editor* editor, core::Key key, bool down)
    {
        if(down) { return; }
        switch(key)
        {
        case core::Key::return_key: editor->tools.pop_tool(); break;
        default: break;
        }
    }


    void
    ToolPlaceMeshOnAPlane::on_scroll(Editor*, const core::Vec2i&)
    {}



    void
    ToolPlaceMeshOnAPlane::on_editor(Editor* editor)
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

