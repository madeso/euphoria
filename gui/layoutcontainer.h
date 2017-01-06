#ifndef GUI_LAYOUTCONTAINER_H
#define GUI_LAYOUTCONTAINER_H

#include "gui/container.h"
#include "core/rect.h"
#include "core/size.h"

#include <memory>
#include <vector>

class Layout;

std::shared_ptr<Layout> CreateTableLayout(const std::vector<bool> expandable_rows, const std::vector<bool> expandable_cols, float combined_padding);
std::shared_ptr<Layout> CreateSingleRowLayout(float padding);

class LayoutContainer : public Container {
public:
  LayoutContainer();
  ~LayoutContainer();

  void SetLayout(std::shared_ptr<Layout> layout);
  std::shared_ptr<Layout> GetLayout();

  void DoLayout(Rectf area);
  Sizef CalculateMinimumArea() const;
private:
  std::shared_ptr<Layout> layout_;
};

#endif  // GUI_LAYOUTCONTAINER_H