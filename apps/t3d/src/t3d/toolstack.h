#pragma once

#include <memory>
#include <vector>

namespace euphoria::t3d
{
    struct Tool;
    struct tool_action;

    struct ToolStack
    {
        Tool*
        get_current_tool();


        void
        push_tool(std::shared_ptr<Tool> new_tool);


        void
        pop_tool();


        void
        perform_tools();


        std::vector<std::shared_ptr<Tool>> current_tool;
        std::vector<std::shared_ptr<tool_action>> pending_actions;
    };
}
