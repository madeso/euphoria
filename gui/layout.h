#ifndef GUI_LAYOUT_H
#define GUI_LAYOUT_H

#include "core/rect.h"

#include <vector>
#include <memory>

class Widget;

class Layout {
public:
  Layout();
  virtual ~Layout();

  virtual Recti CalculateMinimumArea(std::vector<std::shared_ptr<Widget>>& widgets) = 0;
  virtual void DoLayout(std::vector<std::shared_ptr<Widget>>* widgets, const Recti& area) = 0;
};

class TableLayout : public Layout {
public:
  TableLayout(const std::vector<bool> expandable_rows, const std::vector<bool> expandable_cols, int combined_padding);
  virtual Recti CalculateMinimumArea(std::vector<std::shared_ptr<Widget>>& widgets) override;
  virtual void DoLayout(std::vector<std::shared_ptr<Widget>>* widgets, const Recti& area) override;

  std::vector<bool> expandable_rows_;
  std::vector<bool> expandable_cols_;
  int combined_padding_;
};

#endif  // GUI_LAYOUT_H