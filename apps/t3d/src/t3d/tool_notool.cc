#include "t3d/tool_notool.h"


#include "imgui/imgui.h"

#include "t3d/editor.h"


namespace euphoria::t3d
{
    bool
    tool_no_tool::is_busy(editor*)
    {
        return false;
    }


    void
    tool_no_tool::step(editor*)
    {}


    void
    tool_no_tool::on_mouse(editor* editor, core::MouseButton button, bool down)
    {
        if(down) { return; }
        if(button != core::MouseButton::left) { return; }
        auto ray = editor->camera
                        .clip_to_world_ray(
                                editor->viewport.to_clip_coord(editor->mouse))
                        .get_normalized();
        
        editor->set_all_selected(false);
        auto hits = editor->raycast(ray);

        for(auto h: hits)
        {
            h->is_selected = true;
        }
    }


    void
    tool_no_tool::on_key(editor*, core::Key, bool)
    {}


    void
    tool_no_tool::on_scroll(editor*, const core::Vec2i&)
    {}


    void
    tool_no_tool::on_editor(editor*)
    {
        ImGui::Text("<No tool>");
    }
}
