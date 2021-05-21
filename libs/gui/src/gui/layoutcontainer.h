#pragma once

#include "core/rect.h"
#include "core/size2.h"
#include "core/noncopyable.h"

#include "gui/container.h"

#include <memory>
#include <vector>


namespace euphoria::gui
{
    struct layout;


    std::shared_ptr<layout>
    create_table_layout
    (
        const std::vector<bool>& expandable_rows,
        const std::vector<bool>& expandable_cols,
        float combined_padding
    );

    std::shared_ptr<layout>
    create_single_row_layout(float padding);

    struct layout_container : container
    {
        layout_container();
        virtual ~layout_container();

        NONCOPYABLE(layout_container);

        void
        do_layout(core::rectf area);

        [[nodiscard]] core::size2f
        calculate_minimum_area() const;

        std::shared_ptr<gui::layout> layout;
    };
}
