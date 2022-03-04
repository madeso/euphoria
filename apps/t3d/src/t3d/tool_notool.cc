#include "t3d/tool_notool.h"


#include "imgui/imgui.h"

#include "t3d/editor.h"


namespace euphoria::t3d
{
    bool
    ToolNoTool::is_busy(Editor*)
    {
        return false;
    }


    void
    ToolNoTool::step(Editor*)
    {
    }


    void
    ToolNoTool::on_mouse(Editor* editor, core::MouseButton button, bool down)
    {
        if(down) { return; }
        if(button != core::MouseButton::left) { return; }

        auto ray = editor->camera
            .clip_to_world_ray
            (
                editor->viewport.to_clip_coord
                (
                    editor->mouse
                )
            )
            .get_normalized()
            ;
        
        editor->set_all_selected(false);
        auto hits = editor->raycast(ray);

        for(auto h: hits)
        {
            h->is_selected = true;
        }
    }


    void
    ToolNoTool::on_key(Editor*, core::Key, bool)
    {
    }


    void
    ToolNoTool::on_scroll(Editor*, const core::Vec2i&)
    {
    }


    void
    ToolNoTool::on_editor(Editor*)
    {
        ImGui::Text("<No tool>");
    }
}
