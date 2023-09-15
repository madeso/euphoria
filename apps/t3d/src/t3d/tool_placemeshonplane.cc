#include "t3d/tool_placemeshonplane.h"

#include "imgui/imgui.h"

#include "core/intersection.h"
#include "core/numeric.h"

#include "render/actor.h"

#include "t3d/editor.h"
#include "t3d/grid.h"
#include "t3d/tilelibrary.h"

namespace eu::t3d
{
    ToolPlaceMeshOnAPlane::ToolPlaceMeshOnAPlane(std::shared_ptr<PlacedMesh> mesh_to_place)
        : placed_mesh(mesh_to_place)
        , plane(core::Plane::from_normal_and_point(core::common::up, core::zero3f))
    {
    }


    bool
    ToolPlaceMeshOnAPlane::is_busy(Editor*)
    {
        return true;
    }

    void
    ToolPlaceMeshOnAPlane::update(Editor* editor)
    {
        auto ray = editor->camera
            .from_clip_to_world_ray
            (
                editor->viewport.to_clip_coord
                (
                    editor->mouse
                )
            )
            .get_normalized()
            ;

        const auto t = core::get_intersection(ray, plane);

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

        placed_mesh->actor->position = p;
    }


    void
    ToolPlaceMeshOnAPlane::on_mouse(Editor* editor, core::MouseButton button, const KeyboardState&, bool down)
    {
        if(down) { return; }

        switch(button)
        {
        case core::MouseButton::left:
            editor->tools.pop_tool();
            break;

        default:
            break;
        }
    }


    void
    ToolPlaceMeshOnAPlane::on_key(Editor* editor, core::Key key, bool down)
    {
        if(down) { return; }

        switch(key)
        {
        case core::Key::return_key:
            editor->tools.pop_tool();
            break;

        default:
            break;
        }
    }


    void
    ToolPlaceMeshOnAPlane::on_scroll(Editor*, const core::vec2i&)
    {
    }


    void
    ToolPlaceMeshOnAPlane::on_editor(Editor* editor)
    {
        ImGui::Text("Placing object in world!");


        if(editor->selected_tile != nullptr)
        {
            if(editor->tile_library->run_imgui_list(&editor->selected_tile))
            {
                placed_mesh->tile = editor->selected_tile;
                placed_mesh->actor->mesh = editor->selected_tile->mesh;
            }
        }
    }
}

