#include "gui/layout.h"

#include "core/numeric.h"
#include "core/log.h"
#include "core/stringmerger.h"
#include "core/stringutils.h"

#include "gui/widget.h"

#include <numeric>
#include <algorithm>


LOG_SPECIFY_DEFAULT_LOGGER("gui.layout")

Layout::Layout()
{
}

Layout::~Layout()
{
}

TableLayout::TableLayout(
    const std::vector<bool> expandable_rows,
    const std::vector<bool> expandable_cols,
    float                   combined_padding)
    : expandable_rows_(expandable_rows)
    , expandable_cols_(expandable_cols)
    , combined_padding_(combined_padding)
{
}

Sizef
TableLayout::CalculateMinimumArea(
    const std::vector<std::shared_ptr<Widget>>& widgets) const
{
  // todo: include padding
  std::vector<float> width(expandable_cols_.size(), 0);
  std::vector<float> height(expandable_rows_.size(), 0);

  for(auto w : widgets)
  {
    const LayoutData& d = w->layout;
    const Sizef&      s = w->GetPreferredSize();
    UpdateMax(&width[d.GetColumn()], s.GetWidth());
    UpdateMax(&height[d.GetRow()], s.GetHeight());
  }

  const auto s = Sizef::FromWidthHeight(
      std::accumulate(width.begin(), width.end(), 0),
      std::accumulate(height.begin(), height.end(), 0));

  LOG_INFO("Calculate minumum area " << s);

  return s;
}

void
TableLayout::DoLayout(
    std::vector<std::shared_ptr<Widget>>* widgets, const Rectf& area) const
{
  LOG_INFO("Doing table layout in " << area);
  // todo: include padding
  std::vector<float> width(expandable_cols_.size(), 0);
  std::vector<float> height(expandable_rows_.size(), 0);

  for(auto w : *widgets)
  {
    const LayoutData& d = w->layout;
    const Sizef&      s = w->GetPreferredSize();
    UpdateMax(&width[d.GetColumn()], s.GetWidth());
    UpdateMax(&height[d.GetRow()], s.GetHeight());
  }

  LOG_INFO("Table widths: " << StringMerger::Array().Generate(VectorToStringVector(width)));
  LOG_INFO("Table heights: " << StringMerger::Array().Generate(VectorToStringVector(height)));

  const float total_width  = std::accumulate(width.begin(), width.end(), 0);
  const float total_height = std::accumulate(height.begin(), height.end(), 0);

  LOG_INFO("Width " <<  total_width << " height: " << total_height);

  const float leftover_width  = area.GetWidth() - total_width;
  const float leftover_height = area.GetHeight() - total_height;

  const long expandable_rows_count =
      std::count(expandable_rows_.begin(), expandable_rows_.end(), true);
  const long expandable_cols_count =
      std::count(expandable_cols_.begin(), expandable_cols_.end(), true);

  if(expandable_rows_count != 0)
  {
    const float extra = leftover_height / expandable_rows_count;
    for(unsigned int i = 0; i < expandable_rows_.size(); ++i)
    {
      if(expandable_rows_[i])
      {
        height[i] += extra;
      }
    }
  }

  if(expandable_cols_count != 0)
  {
    const float extra = leftover_width / expandable_cols_count;
    for(unsigned int i = 0; i < expandable_cols_.size(); ++i)
    {
      if(expandable_cols_[i])
      {
        width[i] += extra;
      }
    }
  }

  for(auto w : *widgets)
  {
    const LayoutData& d       = w->layout;
    const auto        topleft = area.TopLeft();
    float             x       = topleft.x;
    float             y       = topleft.y;

    LOG_INFO( "widget x " << x << ", y " << y );

    for(int c = 0; c < d.GetColumn(); ++c)
    {
      x += width[c];
    }
    for(int r = 0; r < d.GetRow(); ++r)
    {
      y -= height[r];
    }

    LOG_INFO( "widget x " << x << ", y " << y );

    w->SetRect(Rectf::FromTopLeftWidthHeight(
        y, x, width[d.GetColumn()], height[d.GetRow()]));
  }
}

SingleRowLayout::SingleRowLayout(float padding)
    : padding_(padding)
{
}

Sizef
SingleRowLayout::CalculateMinimumArea(
    const std::vector<std::shared_ptr<Widget>>& widgets) const
{
  float width  = 0;
  float height = 0;
  bool  first  = false;

  for(auto w : widgets)
  {
    const auto& s = w->GetPreferredSize();
    width += s.GetWidth();
    if(!first)
      width += padding_;
    first = false;
    UpdateMax(&height, s.GetHeight());
  }

  return Sizef::FromWidthHeight(width, height);
}

void
SingleRowLayout::DoLayout(
    std::vector<std::shared_ptr<Widget>>* widgets, const Rectf& area) const
{
  LOG_INFO("Doing single row layout in " << area);
  const auto tl = area.TopLeft();
  float      x  = tl.x;
  for(auto w : *widgets)
  {
    const auto& s = w->GetPreferredSize();
    w->SetRect(
        Rectf::FromTopLeftWidthHeight(tl.y, x, s.GetWidth(), s.GetHeight()));
    x += s.GetWidth() + padding_;
  }
}
