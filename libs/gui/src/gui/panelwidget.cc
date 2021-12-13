#include "gui/panelwidget.h"

#include "gui/visitor.h"


namespace euphoria::gui
{
    panel_widget::panel_widget(gui::ui_state* state)
        : widget(state)
    {
    }


    void
    panel_widget::on_size_changed()
    {
        container.do_layout(get_client_rect());
    }


    core::size2f
    panel_widget::calculate_minimum_size() const
    {
        return container.calculate_minimum_area();
    }


    void
    panel_widget::step(float dt)
    {
        container.step(dt);
    }


    void
    panel_widget::render(render::SpriteRenderer* renderer) const
    {
        container.render(renderer);
    }

    void
    panel_widget::visit(visitor* visitor)
    {
        visitor->visit(this);
    }
}

