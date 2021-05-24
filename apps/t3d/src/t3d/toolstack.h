#pragma once

#include <memory>
#include <vector>

namespace euphoria::t3d
{
    struct tool;
    struct tool_action;

    struct tool_stack
    {
        tool*
        get_current_tool();


        void
        push_tool(std::shared_ptr<tool> new_tool);


        void
        pop_tool();


        void
        perform_tools();


        std::vector<std::shared_ptr<tool>>       current_tool;
        std::vector<std::shared_ptr<tool_action>> pending_actions;
    };
}
