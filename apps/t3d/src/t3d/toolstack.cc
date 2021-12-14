#include "t3d/toolstack.h"

#include "core/assert.h"
#include "core/noncopyable.h"


namespace euphoria::t3d
{
    struct tool_action
    {
        tool_action() = default;
        virtual ~tool_action() = default;

        NONCOPYABLE(tool_action);

        virtual void
        act(ToolStack* tools) = 0;
    };


    struct push_tool_action : public tool_action
    {
        std::shared_ptr<Tool> new_tool;


        void
        act(ToolStack* tools) override
        {
            tools->current_tool.push_back(new_tool);
        }


        explicit push_tool_action(std::shared_ptr<Tool> anew_tool)
            : new_tool(anew_tool)
        {
        }
    };


    struct pop_tool_action : public tool_action
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
        pending_actions.push_back(std::make_shared<push_tool_action>(new_tool));
    }


    void
    ToolStack::pop_tool()
    {
        pending_actions.push_back(std::make_shared<pop_tool_action>());
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
