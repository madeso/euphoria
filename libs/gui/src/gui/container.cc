#include "gui/container.h"

#include "gui/widget.h"


namespace euphoria::gui
{
    Container::Container() = default;


    Container::~Container() = default;


    bool
    Container::has_any_widgets() const
    {
        return !widgets.empty();
    }


    void
    Container::add(std::shared_ptr<Widget> widget)
    {
        widgets.push_back(widget);
    }


    void
    Container::step(float dt)
    {
        for(const auto& w: widgets)
        {
            w->step(dt);
        }
    }


    void
    Container::render(render::SpriteRenderer* renderer) const
    {
        for(const std::shared_ptr<Widget>& w: widgets)
        {
            w->render(renderer);
        }
    }
}
