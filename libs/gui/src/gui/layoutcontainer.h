#pragma once

#include "core/rect.h"
#include "core/size2.h"
#include "core/noncopyable.h"

#include "gui/container.h"

#include <memory>



namespace euphoria::gui
{
    struct Layout;


    std::shared_ptr<Layout> create_table_layout
    (
        const std::vector<bool>& expandable_rows,
        const std::vector<bool>& expandable_cols,
        float combined_padding
    );

    std::shared_ptr<Layout> create_single_row_layout(float padding);

    struct LayoutContainer : Container
    {
        std::shared_ptr<gui::Layout> layout;

        LayoutContainer();
        virtual ~LayoutContainer();

        LayoutContainer(const LayoutContainer&) = delete;
        LayoutContainer(LayoutContainer&&) = delete;
        void operator=(const LayoutContainer&) = delete;
        void operator=(LayoutContainer&&) = delete;

        void do_layout(core::Rectf area);

        [[nodiscard]] core::size2f calculate_minimum_area() const;
    };
}
