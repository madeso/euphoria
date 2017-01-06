#ifndef GUI_LAYOUT_H
#define GUI_LAYOUT_H

#include "core/rect.h"
#include "core/size.h"

#include <vector>
#include <memory>

class Widget;

class Layout {
public:
  Layout();
  virtual ~Layout();

  virtual Sizei CalculateMinimumArea(const std::vector<std::shared_ptr<Widget>>& widgets) const = 0;
  virtual void DoLayout(std::vector<std::shared_ptr<Widget>>* widgets, const Recti& area) const = 0;
};

class TableLayout : public Layout {
public:
  TableLayout(const std::vector<bool> expandable_rows, const std::vector<bool> expandable_cols, int combined_padding);
  virtual Sizei CalculateMinimumArea(const std::vector<std::shared_ptr<Widget>>& widgets) const override;
  virtual void DoLayout(std::vector<std::shared_ptr<Widget>>* widgets, const Recti& area) const override;

  std::vector<bool> expandable_rows_;
  std::vector<bool> expandable_cols_;
  int combined_padding_;
};

class SingleRowLayout : public Layout {
public:
  SingleRowLayout(int padding);
  virtual Sizei CalculateMinimumArea(const std::vector<std::shared_ptr<Widget>>& widgets) const override;
  virtual void DoLayout(std::vector<std::shared_ptr<Widget>>* widgets, const Recti& area) const override;

private:
  int padding_;
};

#endif  // GUI_LAYOUT_H