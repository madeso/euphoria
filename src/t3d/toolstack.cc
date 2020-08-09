#include "t3d/toolstack.h"

#include "core/assert.h"


namespace euphoria::t3d
{
    struct ToolAction
    {
        virtual ~ToolAction() = default;


        virtual void
        Act(Tools* tools) = 0;
    };


    struct PushToolAction : public ToolAction
    {
        std::shared_ptr<Tool> new_tool;


        void
        Act(Tools* tools) override
        {
            tools->current_tool.push_back(new_tool);
        }


        explicit PushToolAction(std::shared_ptr<Tool> anew_tool)
            : new_tool(anew_tool)
        {}
    };


    struct PopToolAction : public ToolAction
    {
        void
        Act(Tools* tools) override
        {
            tools->current_tool.pop_back();
        }
    };
    
    
    Tool*
    Tools::GetCurrentTool()
    {
        ASSERT(!current_tool.empty());
        Tool* tool = current_tool.rbegin()->get();
        ASSERT(tool);
        return tool;
    }


    void
    Tools::PushTool(std::shared_ptr<Tool> new_tool)
    {
        pending_actions.push_back(std::make_shared<PushToolAction>(new_tool));
    }


    void
    Tools::PopTool()
    {
        pending_actions.push_back(std::make_shared<PopToolAction>());
    }


    void
    Tools::PerformTools()
    {
        for(auto t: pending_actions)
        {
            t->Act(this);
        }
        pending_actions.resize(0);
    }
}
