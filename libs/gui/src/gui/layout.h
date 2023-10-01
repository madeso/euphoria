#pragma once

#include "base/rect.h"
#include "base/size2.h"


#include <memory>


namespace eu::gui
{
    struct Widget;
}


namespace eu::gui
{
    struct Layout
    {
        Layout();

        virtual
        ~Layout();

        Layout(const Layout& other) = delete;
        void operator=(const Layout&) = delete;
        Layout(Layout&& other) = delete;
        void operator=(Layout&&) = delete;

        [[nodiscard]] virtual size2f
        calc_minimum_area
        (
            const std::vector<std::shared_ptr<Widget>>& widgets
        ) const = 0;

        virtual
        void
        lay_out
        (
            std::vector<std::shared_ptr<Widget>>* widgets,
            const Rectf& area
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

        TableLayout(const TableLayout& other) = delete;
        void operator=(const TableLayout&) = delete;
        TableLayout(TableLayout&& other) = delete;
        void operator=(TableLayout&&) = delete;

        [[nodiscard]] size2f
        calc_minimum_area
        (
            const std::vector<std::shared_ptr<Widget>>& widgets
        ) const override;

        void
        lay_out
        (
            std::vector<std::shared_ptr<Widget>>* widgets,
            const Rectf& area
        ) const override;

        std::vector<bool> expandable_rows;
        std::vector<bool> expandable_cols;
        float combined_padding;
    };

    struct SingleRowLayout : Layout
    {
        SingleRowLayout(float padding);

        ~SingleRowLayout() override = default;

        SingleRowLayout(const SingleRowLayout& other) = delete;
        void operator=(const SingleRowLayout&) = delete;
        SingleRowLayout(SingleRowLayout&& other) = delete;
        void operator=(SingleRowLayout&&) = delete;

        [[nodiscard]] size2f
        calc_minimum_area
        (
            const std::vector<std::shared_ptr<Widget>>& widgets
        ) const override;

        void
        lay_out
        (
            std::vector<std::shared_ptr<Widget>>* widgets,
            const Rectf& area
        ) const override;

        float padding;
    };
}
