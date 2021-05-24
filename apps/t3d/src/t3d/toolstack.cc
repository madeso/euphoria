#include "t3d/toolstack.h"

#include "core/assert.h"


namespace euphoria::t3d
{
    struct tool_action
    {
        virtual ~tool_action() = default;


        virtual void
        act(tool_stack* tools) = 0;
    };


    struct push_tool_action : public tool_action
    {
        std::shared_ptr<tool> new_tool;


        void
        act(tool_stack* tools) override
        {
            tools->current_tool.push_back(new_tool);
        }


        explicit push_tool_action(std::shared_ptr<tool> anew_tool)
            : new_tool(anew_tool)
        {
        }
    };


    struct pop_tool_action : public tool_action
    {
        void
        act(tool_stack* tools) override
        {
            tools->current_tool.pop_back();
        }
    };
    
    
    tool*
    tool_stack::get_current_tool()
    {
        ASSERT(!current_tool.empty());
        tool* tool = current_tool.rbegin()->get();
        ASSERT(tool);
        return tool;
    }


    void
    tool_stack::push_tool(std::shared_ptr<tool> new_tool)
    {
        pending_actions.push_back(std::make_shared<push_tool_action>(new_tool));
    }


    void
    tool_stack::pop_tool()
    {
        pending_actions.push_back(std::make_shared<pop_tool_action>());
    }


    void
    tool_stack::perform_tools()
    {
        for(auto t: pending_actions)
        {
            t->act(this);
        }
        pending_actions.resize(0);
    }
}
