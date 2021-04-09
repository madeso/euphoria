#include "t3d/tool_notool.h"


#include "imgui/imgui.h"

#include "t3d/editor.h"


namespace euphoria::t3d
{
    bool
    NoTool::IsBusy(Editor*)
    {
        return false;
    }


    void
    NoTool::Step(Editor*)
    {}


    void
    NoTool::OnMouse(Editor* editor, core::MouseButton button, bool down)
    {
        if(down) { return; }
        if(button != core::MouseButton::LEFT) { return; }
        auto ray = editor->camera
                        .ClipToWorldRay(
                                editor->viewport.ToClipCoord(editor->mouse))
                        .GetNormalized();
        
        editor->SetAllSelected(false);
        auto hits = editor->Raycast(ray);

        for(auto h: hits)
        {
            h->is_selected = true;
        }
    }


    void
    NoTool::OnKey(Editor*, core::Key, bool)
    {}


    void
    NoTool::OnScroll(Editor*, const core::vec2i&)
    {}


    void
    NoTool::OnEditor(Editor*)
    {
        ImGui::Text("<No tool>");
    }
}
