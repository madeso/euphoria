#ifndef EUPHORIA_T3D_TOOLSTACK_H
#define EUPHORIA_T3D_TOOLSTACK_H

#include <memory>
#include <vector>

namespace euphoria::t3d
{
    struct Tool;
    struct ToolAction;

    struct Tools
    {
        Tool*
        GetCurrentTool();


        void
        PushTool(std::shared_ptr<Tool> new_tool);


        void
        PopTool();


        void
        PerformTools();


        std::vector<std::shared_ptr<Tool>>       current_tool;
        std::vector<std::shared_ptr<ToolAction>> pending_actions;
    };
}

#endif  // EUPHORIA_T3D_TOOLSTACK_H
