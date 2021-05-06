#ifndef EUPHORIA_T3D_TOOL_NOTOOL_H
#define EUPHORIA_T3D_TOOL_NOTOOL_H

#include "t3d/tool.h"

namespace euphoria::t3d
{
    struct NoTool : public Tool
    {
        bool
        IsBusy(Editor*) override;


        void
        Step(Editor*) override;


        void
        OnMouse(Editor*, core::MouseButton, bool) override;


        void
        OnKey(Editor*, core::key, bool) override;


        void
        OnScroll(Editor*, const core::vec2i&) override;


        void
        OnEditor(Editor*) override;
    };
}

#endif  // EUPHORIA_T3D_TOOL_NOTOOL_H
