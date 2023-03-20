#include "gui/layoutcontainer.h"

#include "gui/layout.h"


namespace euphoria::gui
{
    std::shared_ptr<Layout>
    create_table_layout
    (
        const std::vector<bool>& expandable_rows,
        const std::vector<bool>& expandable_cols,
        float combined_padding
    )
    {
        auto ret = std::shared_ptr<Layout>
        {
            new TableLayout
            (
                expandable_rows,
                expandable_cols,
                combined_padding
            )
        };
        return ret;
    }


    std::shared_ptr<Layout>
    create_single_row_layout(float padding)
    {
        auto ret = std::shared_ptr<Layout>{new SingleRowLayout(padding)};
        return ret;
    }


    LayoutContainer::LayoutContainer() = default;


    LayoutContainer::~LayoutContainer() = default;


    void
    LayoutContainer::lay_out(core::Rectf area)
    {
        ASSERT(layout);
        layout->lay_out(&widgets, area);
    }


    core::size2f
    LayoutContainer::calc_minimum_area() const
    {
        ASSERT(layout);
        return layout->calc_minimum_area(widgets);
    }
}
