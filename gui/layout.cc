#include "gui/layout.h"
#include "gui/widget.h"

#include "core/numeric.h"

#include <numeric>

Layout::Layout() {}

Layout::~Layout() {}

TableLayout::TableLayout(const std::vector<bool> expandable_rows, const std::vector<bool> expandable_cols, int combined_padding)
    : expandable_rows_(expandable_rows), expandable_cols_(expandable_cols), combined_padding_(combined_padding) {}

Sizei TableLayout::CalculateMinimumArea(const std::vector<std::shared_ptr<Widget>>& widgets) const {
  // todo: include padding
  std::vector<int> width(expandable_cols_.size(), 0);
  std::vector<int> height(expandable_rows_.size(), 0);

  for (auto w: widgets) {
    const LayoutData& d = w->Layout();
    const Sizei& s = w->GetPreferredSize();
    UpdateMax(&width[d.GetColumn()], s.GetWidth());
    UpdateMax(&height[d.GetRow()], s.GetHeight());
  }

  return Sizei::FromWidthHeight(std::accumulate(width.begin(), width.end(), 0), std::accumulate(height.begin(), height.end(), 0));
}

void TableLayout::DoLayout(std::vector<std::shared_ptr<Widget>>* widgets, const Recti &area) const {
  // todo: include padding
  std::vector<int> width(expandable_cols_.size(), 0);
  std::vector<int> height(expandable_rows_.size(), 0);

  for (auto w: *widgets) {
    const LayoutData& d = w->Layout();
    const Sizei& s = w->GetPreferredSize();
    UpdateMax(&width[d.GetColumn()], s.GetWidth());
    UpdateMax(&height[d.GetRow()], s.GetHeight());
  }

  const int total_width = std::accumulate(width.begin(), width.end(), 0);
  const int total_height = std::accumulate(height.begin(), height.end(), 0);

  const int leftover_width = area.GetWidth() - total_width;
  const int leftover_height = area.GetHeight() - total_height;

  const size_t expandable_rows_count = std::count(expandable_rows_.begin(), expandable_rows_.end(), true);
  const size_t expandable_cols_count = std::count(expandable_cols_.begin(), expandable_cols_.end(), true);

  if(expandable_rows_count != 0) {
    const int extra = leftover_height / expandable_rows_count;
    for (int i = 0;  i < expandable_rows_.size(); ++ i) {
      if( expandable_rows_[i]) {
        height[i] += extra;
      }
    }
  }

  if(expandable_cols_count != 0) {
    const int extra = leftover_width / expandable_cols_count;
    for (int i = 0;  i < expandable_cols_.size(); ++ i) {
      if( expandable_cols_[i]) {
        width[i] += extra;
      }
    }
  }

  for (auto w: *widgets) {
    const LayoutData& d = w->Layout();
    int x = area.GetX();
    int y = area.GetY();

    for(int c=0; c<d.GetColumn(); ++c) {
      x += width[c];
    }
    for(int r=0; r<d.GetRow(); ++r) {
      y += height[r];
    }
    w->SetRect(Recti::FromTopLeftWidthHeight(y, x, width[d.GetColumn()], height[d.GetRow()]));
  }
}

SingleRowLayout::SingleRowLayout(int padding) : padding_(padding) {}

Sizei SingleRowLayout::CalculateMinimumArea(const std::vector<std::shared_ptr<Widget>> &widgets) const {
  int width = 0;
  int height = 0;
  bool first = false;

  for (auto w: widgets) {
    const auto& s = w->GetPreferredSize();
    width += s.GetWidth();
    if( !first ) width += padding_;
    first = false;
    UpdateMax(&height, s.GetHeight());
  }

  return Sizei::FromWidthHeight(width, height);
}

void SingleRowLayout::DoLayout(std::vector<std::shared_ptr<Widget>> *widgets, const Recti &area) const {
  int x = area.GetX();
  for (auto w: *widgets) {
    const auto& s = w->GetPreferredSize();
    w->SetRect( Recti::FromTopLeftWidthHeight(area.GetY(), x, s.GetWidth(), s.GetHeight()) );
    x += s.GetWidth() + padding_;
  }
}
