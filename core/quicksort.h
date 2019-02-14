#ifndef CORE_QUICKSORT_H
#define CORE_QUICKSORT_H

#include <vector>
#include <iostream>

template<typename T>
int Partition(std::vector<T>* arr, int left, int right, const T& pivot)
{
  while(left < right )
  {
    // add sort argument
    while(left < right && (*arr)[left] < pivot)
    {
      left += 1;
    }
    while(left < right && (*arr)[right] > pivot)
    {
      right -= 1;
    }
    if(left < right)
    {
      std::swap((*arr)[left], (*arr)[right]);
      left += 1;
      right -= 1;
    }
  }
  return left;
}


template<typename T>
void QuickSortSub(std::vector<T>* arr, int left, int right)
{
  if(left >= right)
  {
    return;
  }

  const auto pivot_index = left + (right-left)/2;
  if(pivot_index == left) { return; }
  const auto& pivot = (*arr)[pivot_index];
  const auto index = Partition(arr, left, right, pivot);
  // todo: make not recursive?
  QuickSortSub(arr, left, index-1);
  QuickSortSub(arr, index, right);
}

template<typename T>
void QuickSort(std::vector<T>* arr)
{
  QuickSortSub<T>(arr, 0, arr->size()-1);
}

#endif  // CORE_QUICKSORT_H

