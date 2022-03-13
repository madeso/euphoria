#include "t3d/tool_notool.h"

#include "window/imgui_icons.h"
#include "window/imgui_extra.h"

#include "imgui/imgui.h"

#include "t3d/editor.h"
#include "t3d/t3d.h"
#include "t3d/keyboardstate.h"


namespace euphoria::t3d
{
    ToolNoTool::ToolNoTool(Application* a)
        : app(a)
    {
    }


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
    ToolNoTool::on_mouse(Editor* editor, core::MouseButton button, const KeyboardState& state, bool down)
    {
        if (down) { return; }
        if (button != core::MouseButton::left) { return; }

        auto ray = editor->camera.clip_to_world_ray
        (
            editor->viewport.to_clip_coord
            (
                editor->mouse
            )
        )
        .get_normalized()
        ;

        if (state.ctrl == false)
        {
            editor->set_all_selected(false);
        }

        auto hits = editor->raycast(ray);

        for (auto h : hits)
        {
            h->is_selected = !h->is_selected;
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


    bool
    toggle(const char* label, bool down)
    {
        if (down == false)
        {
            const auto disabled = window::imgui::VisuallyDisabledWidgets{};
            return ImGui::Button(label);
        }
        else
        {
            return ImGui::Button(label);
        }
    }

    bool
    toggle(const char* label, bool* state)
    {
        const auto changed = toggle(label, *state);
        if (changed)
        {
            *state = !*state;
        }
        return changed;
    }


    void
    ToolNoTool::on_editor(Editor*)
    {
        ImGui::Text("<No tool>");

        if
        (
            ImGui::Button
            (
                global_space
                ? ICON_MDI_EARTH " global"
                : ICON_MDI_ARROW_DOWN " local"
            )
        )
        {
            global_space = !global_space;
        }
        app->show_help("Toggle the global/local space");

        if (toggle(ICON_MDI_ARROW_ALL, is_translate == true))
        {
            is_translate = true;
        }
        ImGui::SameLine();
        if (toggle(ICON_MDI_ROTATE_ORBIT, is_translate == false))
        {
            is_translate = false;
        }

        if (is_translate)
        {
            toggle(ICON_MDI_AXIS_X_ARROW, &translate_x);
            ImGui::SameLine();
            toggle(ICON_MDI_AXIS_Y_ARROW, &translate_y);
            ImGui::SameLine();
            toggle(ICON_MDI_AXIS_Z_ARROW, &translate_z);
        }
        else
        {
            toggle(ICON_MDI_AXIS_X_ROTATE_CLOCKWISE, &rotate_x);
            ImGui::SameLine();
            toggle(ICON_MDI_AXIS_Y_ROTATE_CLOCKWISE, &rotate_y);
            ImGui::SameLine();
            toggle(ICON_MDI_AXIS_Z_ROTATE_CLOCKWISE, &rotate_z);
        }

        app->guizmo
        (
            is_translate,
            global_space,
            translate_x, translate_y, translate_z,
            rotate_x, rotate_y, rotate_z
        );
    }
}
