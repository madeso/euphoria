#pragma once

#include "core/rect.h"
#include "core/size2.h"
#include "core/noncopyable.h"


#include <memory>


namespace euphoria::gui
{
    struct Widget;
}


namespace euphoria::gui
{
    struct Layout
    {
        Layout();

        virtual
        ~Layout();

        NONCOPYABLE(Layout);

        [[nodiscard]] virtual core::size2f
        calculate_minimum_area
        (
            const std::vector<std::shared_ptr<Widget>>& widgets
        ) const = 0;

        virtual
        void
        do_layout
        (
            std::vector<std::shared_ptr<Widget>>* widgets,
            const core::Rectf& area
        ) const = 0;
    };

    struct TableLayout : Layout
    {
        TableLayout
        (
            const std::vector<bool>& er,
            const std::vector<bool>& ec,
            float cp
        );

        ~TableLayout() override = default;

        NONCOPYABLE(TableLayout);

        [[nodiscard]] core::size2f
        calculate_minimum_area
        (
            const std::vector<std::shared_ptr<Widget>>& widgets
        ) const override;

        void
        do_layout
        (
            std::vector<std::shared_ptr<Widget>>* widgets,
            const core::Rectf& area
        ) const override;

        std::vector<bool> expandable_rows;
        std::vector<bool> expandable_cols;
        float combined_padding;
    };

    struct SingleRowLayout : Layout
    {
        SingleRowLayout(float padding);

        ~SingleRowLayout() override = default;

        NONCOPYABLE(SingleRowLayout);

        [[nodiscard]] core::size2f
        calculate_minimum_area
        (
            const std::vector<std::shared_ptr<Widget>>& widgets
        ) const override;

        void
        do_layout
        (
            std::vector<std::shared_ptr<Widget>>* widgets,
            const core::Rectf& area
        ) const override;

        float padding;
    };
}
