#include "gui/layoutcontainer.h"
#include "gui/layout.h"

#include <cassert>

std::shared_ptr<Layout> CreateTableLayout(const std::vector<bool> expandable_rows, const std::vector<bool> expandable_cols, int combined_padding) {
  std::shared_ptr<Layout> ret { new TableLayout(expandable_rows, expandable_cols, combined_padding) };
  return ret;
}

LayoutContainer::LayoutContainer() {}

LayoutContainer::~LayoutContainer() {}

void LayoutContainer::SetLayout(std::shared_ptr<Layout> layout) {
  assert(this);
  layout_ = layout;
}

std::shared_ptr<Layout> LayoutContainer::GetLayout() {
  assert(this);
  return layout_;
}

void LayoutContainer::DoLayout(Recti area) {
  assert(this);
  assert(layout_.get());
  layout_->DoLayout(&widgets_, area);
}