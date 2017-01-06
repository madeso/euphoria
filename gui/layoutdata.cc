#include "gui/layoutdata.h"

#include <cassert>

LayoutData::LayoutData() {}

LayoutData::~LayoutData() {}

int LayoutData::GetColumn() const {
  assert(this);
  return column_;
}

LayoutData& LayoutData::SetColumn(int c) {
  assert(this);
  column_ = c;
  return *this;
}

int LayoutData::GetRow() const {
  assert(this);
  return row_;
}

LayoutData& LayoutData::SetRow(int r) {
  assert(this);
  row_ = r;
  return *this;
}

float LayoutData::GetPreferredWidth() const {
  assert(this);
  return preferred_width_;
}

LayoutData& LayoutData::SetPreferredWidth(float w) {
  assert(this);
  preferred_width_ = w;
  return *this;
}

float LayoutData::GetPreferredHeight() const {
  assert(this);
  return preferred_height_;
}

LayoutData& LayoutData::SetPreferredHeight(float h) {
  assert(this);
  preferred_height_ = h;
  return *this;
}
