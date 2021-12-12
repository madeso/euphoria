#pragma once

#include "core/rect.h"
#include "core/size2.h"
#include "core/noncopyable.h"

#include <vector>
#include <memory>


namespace euphoria::gui
{
    struct widget;
}


namespace euphoria::gui
{
    struct layout
    {
        layout();

        virtual
        ~layout();

        NONCOPYABLE(layout);

        [[nodiscard]] virtual core::size2f
        calculate_minimum_area
        (
            const std::vector<std::shared_ptr<widget>>& widgets
        ) const = 0;

        virtual
        void
        do_layout
        (
            std::vector<std::shared_ptr<widget>>* widgets,
            const core::Rectf& area
        ) const = 0;
    };

    struct table_layout : layout
    {
        table_layout
        (
            const std::vector<bool>& er,
            const std::vector<bool>& ec,
            float cp
        );

        ~table_layout() override = default;

        NONCOPYABLE(table_layout);

        [[nodiscard]] core::size2f
        calculate_minimum_area
        (
            const std::vector<std::shared_ptr<widget>>& widgets
        ) const override;

        void
        do_layout
        (
            std::vector<std::shared_ptr<widget>>* widgets,
            const core::Rectf& area
        ) const override;

        std::vector<bool> expandable_rows;
        std::vector<bool> expandable_cols;
        float combined_padding;
    };

    struct single_row_layout : layout
    {
        single_row_layout(float padding);

        ~single_row_layout() override = default;

        NONCOPYABLE(single_row_layout);

        [[nodiscard]] core::size2f
        calculate_minimum_area
        (
            const std::vector<std::shared_ptr<widget>>& widgets
        ) const override;

        void
        do_layout
        (
            std::vector<std::shared_ptr<widget>>* widgets,
            const core::Rectf& area
        ) const override;

        float padding;
    };
}
