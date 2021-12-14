#include "gui/widget.h"

#include "core/log.h"

#include "gui/uistate.h"


namespace euphoria::gui
{
    Widget::Widget(gui::State* state)
        : ui_state(state)
    {
    }


    Widget::~Widget() = default;


    bool
    Widget::is_active() const
    {
        return ui_state->active == this;
    }


    bool
    Widget::is_hot() const
    {
        return ui_state->hot == this;
    }


    void
    Widget::on_size_changed()
    {
    }


    core::Rectf
    Widget::get_client_rect() const
    {
        return rect.inset_copy(padding.left, padding.right, padding.top, padding.bottom);
    }


    core::Rectf
    Widget::get_background_rect() const
    {
        return rect;
    }


    void
    Widget::set_rect(const core::Rectf& r)
    {
        rect = r.inset_copy
        (
            margin.left,
            margin.right,
            margin.top,
            margin.bottom
        );
        LOG_INFO("Setting gui rect of '{0}' {1}", name, rect);
        on_size_changed();
    }


    core::size2f
    Widget::get_preferred_size() const
    {
        const auto min = calculate_minimum_size();
        const auto padding_width = padding.left + padding.right;
        const auto padding_height = padding.top + padding.bottom;
        const auto margin_width = margin.left + margin.right;
        const auto margin_height = margin.top + margin.bottom;

        return core::size2f::create_from_width_height
        (
            core::max
            (
                min.width + padding_width + margin_width,
                layout.preferred_width
            ),
            core::max
            (
                min.height + padding_height + margin_height,
                layout.preferred_height
            )
        );
    }


    const State&
    Widget::get_state() const
    {
        return *ui_state;
    }


    State*
    Widget::get_state_ptr() const
    {
        return ui_state;
    }
}
