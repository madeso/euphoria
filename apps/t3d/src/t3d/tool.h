#pragma once

#include "core/vec2.h"
#include "core/key.h"

namespace euphoria::t3d
{
    struct editor;


    struct tool
    {
        virtual ~tool() = default;


        virtual bool
        is_busy(t3d::editor* editor) = 0;


        virtual void
        step(t3d::editor* editor) = 0;


        virtual void
        on_mouse(t3d::editor* editor, core::MouseButton key, bool down) = 0;
        
        
        virtual void
        on_key(t3d::editor* editor, core::key key, bool down) = 0;
        
        
        virtual void
        on_scroll(t3d::editor* editor, const core::vec2i& scroll) = 0;
        
        
        virtual void
        on_editor(t3d::editor* editor) = 0;
    };
}
