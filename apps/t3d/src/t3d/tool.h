#pragma once

#include "core/vec2.h"
#include "core/key.h"
#include "core/noncopyable.h"

namespace euphoria::t3d
{
    struct editor;


    struct tool
    {
        tool() = default;
        virtual ~tool() = default;

        NONCOPYABLE(tool);

        virtual bool
        is_busy(t3d::editor* editor) = 0;


        virtual void
        step(t3d::editor* editor) = 0;


        virtual void
        on_mouse(t3d::editor* editor, core::mouse_button key, bool down) = 0;
        
        
        virtual void
        on_key(t3d::editor* editor, core::key key, bool down) = 0;
        
        
        virtual void
        on_scroll(t3d::editor* editor, const core::vec2i& scroll) = 0;
        
        
        virtual void
        on_editor(t3d::editor* editor) = 0;
    };
}
