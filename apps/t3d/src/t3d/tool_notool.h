#pragma once

#include "t3d/tool.h"

namespace euphoria::t3d
{
    struct tool_no_tool : public tool
    {
        bool
        is_busy(editor*) override;


        void
        step(editor*) override;


        void
        on_mouse(editor*, core::MouseButton, bool) override;


        void
        on_key(editor*, core::key, bool) override;


        void
        on_scroll(editor*, const core::vec2i&) override;


        void
        on_editor(editor*) override;
    };
}
