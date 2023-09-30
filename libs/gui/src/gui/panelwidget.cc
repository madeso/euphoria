#include "gui/panelwidget.h"

#include "gui/visitor.h"


namespace eu::gui
{
    PanelWidget::PanelWidget(gui::State* state)
        : Widget(state)
    {
    }


    void
    PanelWidget::on_size_changed()
    {
        container.lay_out(get_client_rect());
    }


    size2f
    PanelWidget::calc_minimum_size() const
    {
        return container.calc_minimum_area();
    }


    void
    PanelWidget::update(float dt)
    {
        container.update(dt);
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

