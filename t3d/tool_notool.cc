#include "t3d/tool_notool.h"

#include "imgui/imgui.h"

namespace euphoria::t3d
{
    void
    NoTool::MeshHasChanged(Editor*)
    {}


    bool
    NoTool::IsBusy(Editor*)
    {
        return false;
    }


    void
    NoTool::Step(Editor*)
    {}


    void
    NoTool::OnMouse(Editor*, core::MouseButton, bool)
    {}


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
