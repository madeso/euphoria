#pragma once

#include <memory>

#include "t3d/tool.h"
#include "t3d/editor.h"

namespace euphoria::t3d
{
    struct Application;

    struct ActiveSelectionClick
    {
        std::vector<std::shared_ptr<PlacedMesh>> meshes;
        bool ctrl_down;
    };

    struct ToolNoTool : public Tool
    {
        Application* app;
        bool global_space = false;
        bool is_translate = true;

        bool translate_x = true;
        bool translate_y = true;
        bool translate_z = true;

        bool rotate_x = true;
        bool rotate_y = true;
        bool rotate_z = true;

        ActiveSelectionClick active_selection_click;
        bool trigger_selection_popup = false;

        ToolNoTool(Application* a);


        bool
        is_busy(Editor*) override;


        void
        step(Editor*) override;


        void
        on_mouse(Editor*, core::MouseButton, const KeyboardState&, bool) override;


        void
        on_key(Editor*, core::Key, bool) override;


        void
        on_scroll(Editor*, const core::Vec2i&) override;


        void
        on_editor(Editor*) override;
    };
}
