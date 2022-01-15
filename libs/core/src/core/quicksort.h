#pragma once


#include <vector>

#include "core/cint.h"


namespace euphoria::core
{
    // Hoare partition scheme as described in wikipedia
    // https://en.wikipedia.org/wiki/Quicksort
    template <typename T, typename SortFunc>
    int
    get_hoare_partition(SortFunc sort_func, std::vector<T>& array, int lo, int hi)
    {
        const auto pivot_index = lo + (hi - lo) / 2;
        const auto pivot = array[pivot_index];
        auto i = lo - 1;
        auto j = hi + 1;
        while(true)
        {
            do
            {
                i += 1;
            } while(sort_func(array[i], pivot) < 0);
            do
            {
                j -= 1;
            } while(sort_func(array[j], pivot) > 0);

            if(i >= j)
            {
                return j;
            }
            std::swap(array[i], array[j]);
        }
    }


    template <typename T, typename SortFunc>
    void
    quicksort_implementation(SortFunc sort_func, std::vector<T>& array, int lo, int hi)
    {
        if(lo < hi)
        {
            const auto p = get_hoare_partition(sort_func, array, lo, hi);
            quicksort_implementation(sort_func, array, lo, p);
            quicksort_implementation(sort_func, array, p + 1, hi);
        }
    }

    template <typename T, typename SortFunc>
    void
    quicksort(std::vector<T>* array, SortFunc sort_func)
    {
        quicksort_implementation<T, SortFunc>(sort_func, *array, 0, c_sizet_to_int(array->size()) - 1);
    }

    template <typename T, typename SortFunc>
    std::vector<T>
    quicksort(const std::vector<T>& arr, SortFunc sort_func)
    {
        auto copy = arr;
        quicksort(&copy, sort_func);
        return copy;
    }

    template <typename T>
    int
    default_sort_function_for_quicksort(const T& lhs, const T& rhs)
    {
        if(lhs == rhs)
        {
            return 0;
        }
        return lhs < rhs ? -1 : 1;
    }

    template <typename T>
    std::vector<T>
    quicksort(const std::vector<T>& arr)
    {
        return quicksort(arr, default_sort_function_for_quicksort<T>);
    }

}

