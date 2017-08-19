#include "gui/layoutdata.h"

#include "core/assert.h"

LayoutData::LayoutData() {}

LayoutData::~LayoutData() {}

int LayoutData::GetColumn() const {
  ASSERT(this);
  return column_;
}

LayoutData& LayoutData::SetColumn(int c) {
  ASSERT(this);
  column_ = c;
  return *this;
}

int LayoutData::GetRow() const {
  ASSERT(this);
  return row_;
}

LayoutData& LayoutData::SetRow(int r) {
  ASSERT(this);
  row_ = r;
  return *this;
}

float LayoutData::GetPreferredWidth() const {
  ASSERT(this);
  return preferred_width_;
}

LayoutData& LayoutData::SetPreferredWidth(float w) {
  ASSERT(this);
  preferred_width_ = w;
  return *this;
}

float LayoutData::GetPreferredHeight() const {
  ASSERT(this);
  return preferred_height_;
}

LayoutData& LayoutData::SetPreferredHeight(float h) {
  ASSERT(this);
  preferred_height_ = h;
  return *this;
}
