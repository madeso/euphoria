#include "gui/panelwidget.h"

#include "gui/visitor.h"


namespace euphoria::gui
{
    PanelWidget::PanelWidget(gui::State* state)
        : Widget(state)
    {
    }


    void
    PanelWidget::on_size_changed()
    {
        container.do_layout(get_client_rect());
    }


    core::Size2f
    PanelWidget::calculate_minimum_size() const
    {
        return container.calculate_minimum_area();
    }


    void
    PanelWidget::step(float dt)
    {
        container.step(dt);
    }


    void
    PanelWidget::render(render::SpriteRenderer* renderer) const
    {
        container.render(renderer);
    }

    void
    PanelWidget::visit(Visitor* visitor)
    {
        visitor->visit(this);
    }
}

