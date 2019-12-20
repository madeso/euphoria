#include "t3d/tool_placemeshonplane.h"

#include "imgui/imgui.h"

#include "core/intersection.h"

#include "render/actor.h"

#include "t3d/editor.h"


namespace euphoria::t3d
{
    PlaceMeshOnPlane::PlaceMeshOnPlane(std::shared_ptr<render::Actor> aactor)
        : actor(aactor)
        , plane(core::Plane::FromNormalAndPoint(core::unit3f::Up(), core::vec3f::Zero()))
    {}


    void
    PlaceMeshOnPlane::MeshHasChanged(Editor* editor)
    {
        actor->mesh_ = editor->selected_mesh;
    }


    bool
    PlaceMeshOnPlane::IsBusy(Editor*)
    {
        return true;
    }


    void
    PlaceMeshOnPlane::Step(Editor* editor)
    {
        auto ray = editor->camera
                        .ClipToWorldRay(
                                editor->viewport.ToClipCoord(editor->mouse))
                        .GetNormalized();

        const auto t = core::GetIntersection(ray, plane);

        // LOG_INFO("Step " << editor->mouse << ", " << t);

        if(t < 0)
        {
            return;
        }
        // do intersection with plane...
        const auto p = ray.GetPoint(t);
        actor->SetPosition(p);
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
    PlaceMeshOnPlane::OnEditor(Editor*)
    {
        ImGui::Text("Placing object in world!");
    }
}

