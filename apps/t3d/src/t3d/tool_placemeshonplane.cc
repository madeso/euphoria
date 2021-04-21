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
    PlaceMeshOnPlane::PlaceMeshOnPlane(std::shared_ptr<PlacedMesh> aactor)
        : actor(aactor)
        , plane(core::plane::from_normal_and_point(core::unit3f::up(), core::vec3f::zero()))
    {}


    bool
    PlaceMeshOnPlane::IsBusy(Editor*)
    {
        return true;
    }


    float
    SnapTo(float val, float step)
    {
        return core::Round(val, step);
    }


    void
    PlaceMeshOnPlane::Step(Editor* editor)
    {
        auto ray = editor->camera
                        .ClipToWorldRay(
                                editor->viewport.ToClipCoord(editor->mouse))
                        .get_normalized();

        const auto t = core::GetIntersection(ray, plane);

        // LOG_INFO("Step " << editor->mouse << ", " << t);

        if(t < 0)
        {
            return;
        }
        // do intersection with plane...
        auto p = ray.get_point(t);

        if(editor->grid->snap_enabled)
        {
            p.x = SnapTo(p.x, editor->grid->small_step);
            p.z = SnapTo(p.z, editor->grid->small_step);
        }

        actor->actor->SetPosition(p);
    }


    void
    PlaceMeshOnPlane::OnMouse(Editor* editor, core::MouseButton button, bool down)
    {
        if(down)
            return;
        switch(button)
        {
        case core::MouseButton::LEFT: editor->tools.PopTool(); break;
        default: break;
        }
    }


    void
    PlaceMeshOnPlane::OnKey(Editor* editor, core::Key key, bool down)
    {
        if(down)
            return;
        switch(key)
        {
        case core::Key::RETURN: editor->tools.PopTool(); break;
        default: break;
        }
    }


    void
    PlaceMeshOnPlane::OnScroll(Editor*, const core::vec2i&)
    {}



    void
    PlaceMeshOnPlane::OnEditor(Editor* editor)
    {
        ImGui::Text("Placing object in world!");


        if(editor->selected_mesh != nullptr)
        {
            if(editor->tile_library->ImGuiList(&editor->selected_mesh))
            {
                actor->tile = editor->selected_mesh;
                actor->actor->mesh_ = editor->selected_mesh->mesh;
            }
        }
    }
}

