#include "gui/layoutcontainer.h"
#include "gui/layout.h"

#include "core/assert.h"

std::shared_ptr<Layout> CreateTableLayout(const std::vector<bool> expandable_rows, const std::vector<bool> expandable_cols, float combined_padding) {
  std::shared_ptr<Layout> ret { new TableLayout(expandable_rows, expandable_cols, combined_padding) };
  return ret;
}

std::shared_ptr<Layout> CreateSingleRowLayout(float padding) {
  std::shared_ptr<Layout> ret { new SingleRowLayout(padding) };
  return ret;
}

LayoutContainer::LayoutContainer() {}

LayoutContainer::~LayoutContainer() {}

void LayoutContainer::SetLayout(std::shared_ptr<Layout> layout) {
  ASSERT(this);
  layout_ = layout;
}

std::shared_ptr<Layout> LayoutContainer::GetLayout() {
  ASSERT(this);
  return layout_;
}

void LayoutContainer::DoLayout(Rectf area) {
  ASSERT(this);
  ASSERT(layout_.get());
  layout_->DoLayout(&widgets_, area);
}

Sizef LayoutContainer::CalculateMinimumArea() const {
  ASSERT(this);
  ASSERT(layout_.get());
  return layout_->CalculateMinimumArea(widgets_);
}
