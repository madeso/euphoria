#pragma once

#include <memory>

#include "core/rect.h"
#include "base/size2.h"

#include "gui/container.h"



namespace eu::gui
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

        void lay_out(core::Rectf area);

        [[nodiscard]] size2f calc_minimum_area() const;
    };
}
