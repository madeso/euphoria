#pragma once

#include "core/rect.h"
#include "core/size.h"
#include "core/noncopyable.h"

#include "gui/container.h"

#include <memory>
#include <vector>


namespace euphoria::gui
{
    struct Layout;
}


namespace euphoria::gui
{
    std::shared_ptr<Layout>
    CreateTableLayout
    (
        const std::vector<bool>& expandable_rows,
        const std::vector<bool>& expandable_cols,
        float combined_padding
    );

    std::shared_ptr<Layout>
    CreateSingleRowLayout(float padding);

    struct LayoutContainer : public Container
    {
        LayoutContainer();
        virtual ~LayoutContainer();

        NONCOPYABLE(LayoutContainer);

        // todo(Gustav): remove
        void
        SetLayout(std::shared_ptr<Layout> layout);

        // todo(Gustav): remove
        [[nodiscard]] std::shared_ptr<Layout>
        GetLayout() const;

        void
        DoLayout(core::rectf area);

        [[nodiscard]] core::Sizef
        CalculateMinimumArea() const;

        std::shared_ptr<Layout> layout_;
    };
}
