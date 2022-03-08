#pragma once

#include "t3d/tool.h"

namespace euphoria::t3d
{
    struct Application;

    struct ToolNoTool : public Tool
    {
        Application* app;
        bool global_space = false;
        bool is_transform = true;

        ToolNoTool(Application* a);


        bool
        is_busy(Editor*) override;


        void
        step(Editor*) override;


        void
        on_mouse(Editor*, core::MouseButton, bool) override;


        void
        on_key(Editor*, core::Key, bool) override;


        void
        on_scroll(Editor*, const core::Vec2i&) override;


        void
        on_editor(Editor*) override;
    };
}
