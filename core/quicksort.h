#ifndef CORE_QUICKSORT_H
#define CORE_QUICKSORT_H

#include <vector>

// Hoare partition scheme as described in wikipedia
// https://en.wikipedia.org/wiki/Quicksort
template<typename T, typename SortFunc>
int Partition(SortFunc sort_func, std::vector<T>& A, int lo, int hi)
{
    const auto pivot_index = lo + (hi-lo)/2;
    const auto pivot = A[pivot_index];
    auto i = lo - 1;
    auto j = hi + 1;
    while(true)
    {
      do {
        i += 1;
      } while(sort_func(A[i], pivot) < 0);
      do {
        j -= 1;
      } while(sort_func(A[j], pivot) > 0);

      if(i >= j)
      {
        return j;
      }
      std::swap(A[i], A[j]);
    }
}


template<typename T, typename SortFunc>
void QuickSortSub(SortFunc sort_func, std::vector<T>& A, int lo, int hi)
{
    if(lo < hi)
    {
        const auto p = Partition(sort_func, A, lo, hi);
        QuickSortSub(sort_func, A, lo, p);
        QuickSortSub(sort_func, A, p+1, hi);
    }
}

template<typename T, typename SortFunc>
void QuickSort(std::vector<T>* arr, SortFunc sort_func)
{
  QuickSortSub<T, SortFunc>(sort_func, *arr, 0, arr->size()-1);
}

template<typename T, typename SortFunc>
std::vector<T> QuickSort(const std::vector<T>& arr, SortFunc sort_func)
{
  auto copy = arr;
  QuickSort(&copy, sort_func);
  return copy;
}

template<typename T>
int DefaultQuickSortFunction(const T& lhs, const T& rhs)
{
  if(lhs == rhs) return 0;
  return lhs < rhs ? -1 : 1;
}

template<typename T>
std::vector<T> QuickSort(const std::vector<T>& arr)
{
  return QuickSort(arr, DefaultQuickSortFunction<T>);
}

#endif  // CORE_QUICKSORT_H

