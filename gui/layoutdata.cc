#include "gui/layoutdata.h"

#include "core/assert.h"

LayoutData::LayoutData() {}

LayoutData::~LayoutData() {}

int LayoutData::GetColumn() const {
  Assert(this);
  return column_;
}

LayoutData& LayoutData::SetColumn(int c) {
  Assert(this);
  column_ = c;
  return *this;
}

int LayoutData::GetRow() const {
  Assert(this);
  return row_;
}

LayoutData& LayoutData::SetRow(int r) {
  Assert(this);
  row_ = r;
  return *this;
}

float LayoutData::GetPreferredWidth() const {
  Assert(this);
  return preferred_width_;
}

LayoutData& LayoutData::SetPreferredWidth(float w) {
  Assert(this);
  preferred_width_ = w;
  return *this;
}

float LayoutData::GetPreferredHeight() const {
  Assert(this);
  return preferred_height_;
}

LayoutData& LayoutData::SetPreferredHeight(float h) {
  Assert(this);
  preferred_height_ = h;
  return *this;
}
