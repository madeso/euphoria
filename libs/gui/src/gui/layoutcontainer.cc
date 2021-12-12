#include "gui/layoutcontainer.h"

#include "gui/layout.h"


namespace euphoria::gui
{
    std::shared_ptr<layout>
    create_table_layout
    (
        const std::vector<bool>& expandable_rows,
        const std::vector<bool>& expandable_cols,
        float combined_padding
    )
    {
        auto ret = std::shared_ptr<layout>
        {
            new table_layout
            (
                expandable_rows,
                expandable_cols,
                combined_padding
            )
        };
        return ret;
    }


    std::shared_ptr<layout>
    create_single_row_layout(float padding)
    {
        auto ret = std::shared_ptr<layout>{new single_row_layout(padding)};
        return ret;
    }


    layout_container::layout_container() = default;


    layout_container::~layout_container() = default;


    void
    layout_container::do_layout(core::Rectf area)
    {
        ASSERT(layout);
        layout->do_layout(&widgets, area);
    }


    core::size2f
    layout_container::calculate_minimum_area() const
    {
        ASSERT(layout);
        return layout->calculate_minimum_area(widgets);
    }
}
