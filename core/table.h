#ifndef CORE_TABLE_H
#define CORE_TABLE_H

#include <vector>

#include "core/assert.h"
#include "core/numeric.h"
#include "core/rect.h"

namespace euphoria::core
{

template <typename T>
struct Table
{
  using I = unsigned int;

  Table()
      : width(0)
      , height(0)
  {
  }

  static Table
  FromWidthHeight(I c, I r, T d = T())
  {
    return Table(c, r, d);
  }

  static Table
  FromHeightWidth(I r, I c, T d = T())
  {
    return Table(c, r, d);
  }

  void
  NewRow(T d = T())
  {
    ASSERT(width > 0);
    data.resize(data.size() + width, d);
    height += 1;
  }

  void
  Clear(T d = T())
  {
    for(size_t i=0; i<data.size(); i+=1)
    {
      data[i] = d;
    }
  }

  template<typename Func>
  void
  SetAll(Func f)
  {
    for(unsigned y=0; y<height; y+=1)
    {
      for(unsigned x=0; x<width; x+=1)
      {
        Value(x, y, f(x, y));
      }
    }
  }

  const Recti
  Indices() const
  {
    return Recti::FromWidthHeight(width-1, height-1);
  }

  void
  NewRow(const std::vector<T>& row)
  {
    ASSERT(row.size() == width || (width == 0 && row.size() > 0));
    I added = 0;
    for(const T& t : row)
    {
      data.push_back(t);
      ++added;
    }
    height += 1;
    if(width == 0)
    {
      width = added;
    }
  }

  // NewCol function??? probably less useful than insert functions

  // todo: insert before/after row/col
  // todo: remove row/col

  void
  Value(I x, I y, const T& t)
  {
    const auto index = DataIndex(x, y);
    ASSERTX(index < data.size(), index, data.size());
    data[index] = t;
  }

  T
  Value(I x, I y) const
  {
    const auto index = DataIndex(x, y);
    ASSERTX(index < data.size(), index, data.size());
    return data[index];
  }

  T&
  RefValue(I x, I y)
  {
    const auto index = DataIndex(x, y);
    ASSERTX(index < data.size(), index, data.size());
    return data[index];
  }

  T*
  PtrValue(I x, I y)
  {
    const auto index = DataIndex(x, y);
    ASSERTX(index < data.size(), index, data.size());
    return &data[index];
  }

  // todo: figure out a better name
  I
  Width() const
  {
    return width;
  }

  I
  Height() const
  {
    return height;
  }

  size_t
  DataIndex(I x, I y) const
  {
    ASSERTX(
        IsWithinInclusivei(0, x, width - 1) &&
            IsWithinInclusivei(0, y, height - 1),
        x,
        width,
        y,
        height);
    return y * width + x;
  }

 private:
  Table(I c, I r, T d = T())
      : data(c * r, d)
      , width(c)
      , height(r)
  {
    ASSERTX(width >= 0 && height >= 0, width, height);
  }

  // table is stored in in place like [row, row, ...row]
  std::vector<T> data;
  I              width;
  I              height;
};

template <typename T>
std::vector<T>
CalcColumnAsVector(const Table<T>& t, typename Table<T>::I x)
{
  ASSERTX(x < t.Width(), x, t.Width());
  std::vector<T> r;
  r.reserve(t.Height());
  for(typename Table<T>::I y = 0; y < t.Height(); ++y)
  {
    r.emplace_back(t.Value(x, y));
  }
  return r;
}

template <typename T>
std::vector<T>
CalcRowAsVector(const Table<T>& t, typename Table<T>::I y)
{
  ASSERTX(y < t.Height(), y, t.Height());

  std::vector<T> r;
  r.reserve(t.Width());
  for(typename Table<T>::I x = 0; x < t.Width(); ++x)
  {
    r.emplace_back(t.Value(x, y));
  }
  return r;
}

}

#endif  // CORE_STRING_TABLE_H
