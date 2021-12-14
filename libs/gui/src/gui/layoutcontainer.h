#pragma once

#include "core/rect.h"
#include "core/size2.h"
#include "core/noncopyable.h"

#include "gui/container.h"

#include <memory>
#include <vector>


namespace euphoria::gui
{
    struct Layout;


    std::shared_ptr<Layout>
    create_table_layout
    (
        const std::vector<bool>& expandable_rows,
        const std::vector<bool>& expandable_cols,
        float combined_padding
    );

    std::shared_ptr<Layout>
    create_single_row_layout(float padding);

    struct LayoutContainer : Container
    {
        LayoutContainer();
        virtual ~LayoutContainer();

        NONCOPYABLE(LayoutContainer);

        void
        do_layout(core::Rectf area);

        [[nodiscard]] core::size2f
        calculate_minimum_area() const;

        std::shared_ptr<gui::Layout> layout;
    };
}
