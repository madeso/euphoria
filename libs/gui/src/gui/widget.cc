#include "gui/widget.h"

#include "log/log.h"

#include "gui/uistate.h"


namespace eu::gui
{
    Widget::Widget(gui::State* state)
        : margin(0.0f)
        , padding(0.0f)
        , ui_state(state)
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


    Rectf
    Widget::get_client_rect() const
    {
        return rect.inset_copy(padding.left, padding.right, padding.up, padding.down);
    }


    Rectf
    Widget::get_background_rect() const
    {
        return rect;
    }


    void
    Widget::set_rect(const Rectf& r)
    {
        rect = r.inset_copy
        (
            margin.left,
            margin.right,
            margin.up,
            margin.down
        );
        on_size_changed();
    }


    size2f
    Widget::get_preferred_size() const
    {
        const auto min = calc_minimum_size();
        const auto padding_width = padding.left + padding.right;
        const auto padding_height = padding.up + padding.down;
        const auto margin_width = margin.left + margin.right;
        const auto margin_height = margin.up + margin.down;

        return size2f::create_from_width_height
        (
            max
            (
                min.width + padding_width + margin_width,
                layout.preferred_width
            ),
            max
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
