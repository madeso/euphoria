#include "gui/layoutdata.h"

LayoutData::LayoutData()
{
}

LayoutData::~LayoutData()
{
}

int
LayoutData::GetColumn() const
{
  return column_;
}

LayoutData&
LayoutData::SetColumn(int c)
{
  column_ = c;
  return *this;
}

int
LayoutData::GetRow() const
{
  return row_;
}

LayoutData&
LayoutData::SetRow(int r)
{
  row_ = r;
  return *this;
}

float
LayoutData::GetPreferredWidth() const
{
  return preferred_width_;
}

LayoutData&
LayoutData::SetPreferredWidth(float w)
{
  preferred_width_ = w;
  return *this;
}

float
LayoutData::GetPreferredHeight() const
{
  return preferred_height_;
}

LayoutData&
LayoutData::SetPreferredHeight(float h)
{
  preferred_height_ = h;
  return *this;
}
