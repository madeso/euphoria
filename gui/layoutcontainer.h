#ifndef GUI_LAYOUTCONTAINER_H
#define GUI_LAYOUTCONTAINER_H

#include "gui/container.h"
#include "core/rect.h"
#include "core/size.h"

#include <memory>
#include <vector>


namespace euphoria::gui
{
    struct Layout;

    std::shared_ptr<Layout>
    CreateTableLayout
    (
        const std::vector<bool> expandable_rows,
        const std::vector<bool> expandable_cols,
        float combined_padding
    );

    std::shared_ptr<Layout>
    CreateSingleRowLayout(float padding);

    struct LayoutContainer : public Container
    {
        LayoutContainer();

        virtual ~LayoutContainer();

        // todo(Gustav): remove
        void
        SetLayout(std::shared_ptr<Layout> layout);

        std::shared_ptr<Layout>
        GetLayout();

        void
        DoLayout(core::Rectf area);

        core::Sizef
        CalculateMinimumArea() const;

        std::shared_ptr<Layout> layout_;
    };
}

#endif  // GUI_LAYOUTCONTAINER_H

