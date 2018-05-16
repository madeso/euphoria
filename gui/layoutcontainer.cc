#include "gui/layoutcontainer.h"
#include "gui/layout.h"

std::shared_ptr<Layout>
CreateTableLayout(
    const std::vector<bool> expandable_rows,
    const std::vector<bool> expandable_cols,
    float                   combined_padding)
{
  std::shared_ptr<Layout> ret{
      new TableLayout(expandable_rows, expandable_cols, combined_padding)};
  return ret;
}

std::shared_ptr<Layout>
CreateSingleRowLayout(float padding)
{
  std::shared_ptr<Layout> ret{new SingleRowLayout(padding)};
  return ret;
}

LayoutContainer::LayoutContainer()
{
}

LayoutContainer::~LayoutContainer()
{
}

void
LayoutContainer::SetLayout(std::shared_ptr<Layout> layout)
{
  layout_ = layout;
}

std::shared_ptr<Layout>
LayoutContainer::GetLayout()
{
  return layout_;
}

void
LayoutContainer::DoLayout(Rectf area)
{
  ASSERT(layout_);
  layout_->DoLayout(&widgets_, area);
}

Sizef
LayoutContainer::CalculateMinimumArea() const
{
  ASSERT(layout_);
  return layout_->CalculateMinimumArea(widgets_);
}
