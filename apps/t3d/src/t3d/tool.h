#pragma once

#include "core/vec2.h"
#include "core/key.h"
#include "core/noncopyable.h"

namespace euphoria::t3d
{
    struct Editor;
    struct KeyboardState;

    struct Tool
    {
        Tool() = default;
        virtual ~Tool() = default;

        NONCOPYABLE(Tool);

        virtual bool
        is_busy(t3d::Editor* editor) = 0;


        virtual void
        step(t3d::Editor* editor) = 0;


        virtual void
        on_mouse(t3d::Editor* editor, core::MouseButton key, const KeyboardState& state, bool down) = 0;
        
        
        virtual void
        on_key(t3d::Editor* editor, core::Key key, bool down) = 0;
        
        
        virtual void
        on_scroll(t3d::Editor* editor, const core::vec2i& scroll) = 0;
        
        
        virtual void
        on_editor(t3d::Editor* editor) = 0;
    };
}
