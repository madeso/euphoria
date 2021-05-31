#include "gui/container.h"

#include "gui/widget.h"


namespace euphoria::gui
{
    container::container() = default;


    container::~container() = default;


    bool
    container::has_any_widgets() const
    {
        return !widgets.empty();
    }


    void
    container::add(std::shared_ptr<widget> widget)
    {
        widgets.push_back(widget);
    }


    void
    container::step(float dt)
    {
        for(const auto& w: widgets)
        {
            w->step(dt);
        }
    }


    void
    container::render(render::sprite_renderer* renderer) const
    {
        for(const std::shared_ptr<widget>& w: widgets)
        {
            w->render(renderer);
        }
    }
}
