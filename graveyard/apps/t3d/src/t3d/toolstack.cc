#include "t3d/toolstack.h"

#include "assert/assert.h"


namespace eu::t3d
{
    struct tool_action
    {
        tool_action() = default;
        virtual ~tool_action() = default;

        tool_action(const tool_action& other) = delete;
        void operator=(const tool_action&) = delete;
        tool_action(tool_action&& other) = delete;
        void operator=(tool_action&&) = delete;

        virtual void
        act(ToolStack* tools) = 0;
    };


    struct PushToolAction : public tool_action
    {
        std::shared_ptr<Tool> new_tool;


        void
        act(ToolStack* tools) override
        {
            tools->current_tool.push_back(new_tool);
        }


        explicit PushToolAction(std::shared_ptr<Tool> anew_tool)
            : new_tool(anew_tool)
        {
        }
    };


    struct PopToolAction : public tool_action
    {
        void
        act(ToolStack* tools) override
        {
            tools->current_tool.pop_back();
        }
    };
    
    
    Tool*
    ToolStack::get_current_tool()
    {
        ASSERT(!current_tool.empty());
        Tool* tool = current_tool.rbegin()->get();
        ASSERT(tool);
        return tool;
    }


    void
    ToolStack::push_tool(std::shared_ptr<Tool> new_tool)
    {
        pending_actions.push_back(std::make_shared<PushToolAction>(new_tool));
    }


    void
    ToolStack::pop_tool()
    {
        pending_actions.push_back(std::make_shared<PopToolAction>());
    }


    void
    ToolStack::perform_tools()
    {
        for(auto t: pending_actions)
        {
            t->act(this);
        }
        pending_actions.resize(0);
    }
}
