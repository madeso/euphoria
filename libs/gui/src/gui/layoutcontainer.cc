#include "gui/layoutcontainer.h"

#include "gui/layout.h"


namespace euphoria::gui
{
    std::shared_ptr<Layout>
    CreateTableLayout
    (
        const std::vector<bool>& expandable_rows,
        const std::vector<bool>& expandable_cols,
        float combined_padding
    )
    {
        auto ret = std::shared_ptr<Layout>
        {
            new TableLayout
            (
                expandable_rows,
                expandable_cols,
                combined_padding
            )
        };
        return ret;
    }


    std::shared_ptr<Layout>
    CreateSingleRowLayout(float padding)
    {
        auto ret = std::shared_ptr<Layout>{new SingleRowLayout(padding)};
        return ret;
    }


    LayoutContainer::LayoutContainer() = default;


    LayoutContainer::~LayoutContainer() = default;


    void
    LayoutContainer::SetLayout(std::shared_ptr<Layout> layout)
    {
        layout_ = layout;
    }


    std::shared_ptr<Layout>
    LayoutContainer::GetLayout() const
    {
        return layout_;
    }


    void
    LayoutContainer::DoLayout(core::Rectf area)
    {
        ASSERT(layout_);
        layout_->DoLayout(&widgets_, area);
    }


    core::Sizef
    LayoutContainer::CalculateMinimumArea() const
    {
        ASSERT(layout_);
        return layout_->CalculateMinimumArea(widgets_);
    }
}
