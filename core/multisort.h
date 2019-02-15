#ifndef CORE_MULTISORT_H
#define CORE_MULTISORT_H

#include <vector>
#include <string>
#include <numeric>
#include <algorithm>
#include <memory>

#include "core/quicksort.h"

enum class SortStyle
{
  Ascending,
  Descending
};

template <typename T>
struct Sortable
{
  virtual ~Sortable() {}
  virtual int
  Sort(const T& lhs, const T& rhs) = 0;
};

template <typename T>
using SortableList = std::vector<std::shared_ptr<Sortable<T>>>;

template <typename T, typename Value, typename SortFunc>
struct SortAction : public Sortable<T>
{
  Value T::*member;
  SortStyle sort_style;
  SortFunc  sort_func;

  SortAction(Value T::*m, SortStyle s, SortFunc f)
      : member(m)
      , sort_style(s)
      , sort_func(f)
  {
  }

  int
  Sort(const T& lhs, const T& rhs) override
  {
    const int result = sort_func(lhs.*member, rhs.*member);
    return sort_style == SortStyle::Ascending ? result : -result;
  }
};

template <typename T>
int
DefaultSortFunc(T lhs, T rhs)
{
  if(lhs == rhs)
    return 0;
  return lhs < rhs ? 1 : -1;
}

template <typename T>
struct SortBuilder
{
  SortableList<T> order;

  template <typename SortFunc, typename Value>
  SortBuilder<T>&
  Add(Value T::*member,
      SortFunc  sort_func,
      SortStyle sort_style = SortStyle::Ascending)
  {
    auto o = std::make_shared<SortAction<T, Value, SortFunc>>(
        member, sort_style, sort_func);
    order.emplace_back(o);
    return *this;
  }

  template <typename Value>
  SortBuilder<T>&
  Add(Value T::*member, SortStyle sort_style = SortStyle::Ascending)
  {
    return Add(member, &DefaultSortFunc<Value>, sort_style);
  }
};

template <typename T>
std::vector<size_t>
GetSortedIndices(const std::vector<T>& data, const SortableList<T>& sort_order)
{
  std::vector<size_t> r(data.size());
  std::iota(std::begin(r), std::end(r), 0);
  // todo: std::sort doesn't seem to work
  QuickSort(&r, [&data, &sort_order](size_t lhs, size_t rhs) -> int {
    for(const auto& so : sort_order)
    {
      const auto result = so->Sort(data[lhs], data[rhs]);
      if(result != 0)
        return -result;
    }
    return 0;
  });
  return r;
}

#endif  // CORE_MULTISORT_H
