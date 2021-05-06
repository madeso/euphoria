#ifndef EUPHORIA_T3D_TOOL_H
#define EUPHORIA_T3D_TOOL_H

#include "core/vec2.h"
#include "core/key.h"

namespace euphoria::t3d
{
    struct Editor;


    struct Tool
    {
        virtual ~Tool() = default;


        virtual bool
        IsBusy(Editor* editor) = 0;


        virtual void
        Step(Editor* editor) = 0;


        virtual void
        OnMouse(Editor* editor, core::MouseButton key, bool down) = 0;
        
        
        virtual void
        OnKey(Editor* editor, core::key key, bool down) = 0;
        
        
        virtual void
        OnScroll(Editor* editor, const core::vec2i& scroll) = 0;
        
        
        virtual void
        OnEditor(Editor* editor) = 0;
    };
}

#endif  // EUPHORIA_T3D_TOOL_H
