#ifndef CORE_QUICKSORT_H
#define CORE_QUICKSORT_H

#include <vector>
#include <iostream>


// Hoare partition scheme as described in wikipedia
// https://en.wikipedia.org/wiki/Quicksort
template<typename T>
int Partition(std::vector<T>& A, int lo, int hi)
{
    const auto pivot_index = lo + (hi-lo)/2;
    const auto pivot = A[pivot_index];
    auto i = lo - 1;
    auto j = hi + 1;
    while(true)
    {
      do {
        i += 1;
      } while(A[i] < pivot);
      do {
        j -= 1;
      } while(A[j] > pivot);

      if(i >= j)
      {
        return j;
      }
      std::swap(A[i], A[j]);
    }
}


template<typename T>
void QuickSortSub(std::vector<T>& A, int lo, int hi)
{
    if(lo < hi)
    {
        const auto p = Partition(A, lo, hi);
        QuickSortSub(A, lo, p);
        QuickSortSub(A, p+1, hi);
    }
}

template<typename T>
void QuickSort(std::vector<T>* arr)
{
  QuickSortSub<T>(*arr, 0, arr->size()-1);
}

template<typename T>
std::vector<T> QuickSort(const std::vector<T>& arr)
{
  auto copy = arr;
  QuickSort(&copy);
  return copy;
}

#endif  // CORE_QUICKSORT_H

