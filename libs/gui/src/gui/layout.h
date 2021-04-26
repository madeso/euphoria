#pragma once

#include "core/rect.h"
#include "core/size.h"
#include "core/noncopyable.h"

#include <vector>
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

        [[nodiscard]] virtual core::Sizef
        CalculateMinimumArea
        (
            const std::vector<std::shared_ptr<Widget>>& widgets
        ) const = 0;

        virtual
        void
        DoLayout
        (
            std::vector<std::shared_ptr<Widget>>* widgets,
            const core::rectf& area
        ) const = 0;
    };

    struct TableLayout : public Layout
    {
        TableLayout
        (
            const std::vector<bool>& expandable_rows,
            const std::vector<bool>& expandable_cols,
            float combined_padding
        );

        ~TableLayout() override = default;

        NONCOPYABLE(TableLayout);

        [[nodiscard]] core::Sizef
        CalculateMinimumArea
        (
            const std::vector<std::shared_ptr<Widget>>& widgets
        ) const override;

        void
        DoLayout
        (
            std::vector<std::shared_ptr<Widget>>* widgets,
            const core::rectf& area
        ) const override;

        std::vector<bool> expandable_rows_;
        std::vector<bool> expandable_cols_;
        float combined_padding_;
    };

    struct SingleRowLayout : public Layout
    {
        SingleRowLayout(float padding);

        ~SingleRowLayout() override = default;

        NONCOPYABLE(SingleRowLayout);

        [[nodiscard]] core::Sizef
        CalculateMinimumArea
        (
            const std::vector<std::shared_ptr<Widget>>& widgets
        ) const override;

        void
        DoLayout
        (
            std::vector<std::shared_ptr<Widget>>* widgets,
            const core::rectf& area
        ) const override;

        float padding_;
    };
}
