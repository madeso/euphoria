#pragma once




#include "core/cint.h"


namespace euphoria::core
{
    // Hoare partition scheme as described in wikipedia
    // https://en.wikipedia.org/wiki/Quicksort
    template <typename T, typename TSortFunc>
    int
    get_hoare_partition(TSortFunc sort_func, std::vector<T>& array, int lo, int hi)
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


    template <typename T, typename TSortFunc>
    void
    do_inplace_quick_sort_impl(TSortFunc sort_func, std::vector<T>& array, int lo, int hi)
    {
        if(lo < hi)
        {
            const auto p = get_hoare_partition(sort_func, array, lo, hi);
            do_inplace_quick_sort_impl(sort_func, array, lo, p);
            do_inplace_quick_sort_impl(sort_func, array, p + 1, hi);
        }
    }

    template <typename T, typename TSortFunc>
    void
    do_inplace_quick_sort(std::vector<T>* array, TSortFunc sort_func)
    {
        do_inplace_quick_sort_impl<T, TSortFunc>(sort_func, *array, 0, c_sizet_to_int(array->size()) - 1);
    }

    template <typename T, typename TSortFunc>
    std::vector<T>
    get_quick_sorted(const std::vector<T>& arr, TSortFunc sort_func)
    {
        auto copy = arr;
        do_inplace_quick_sort(&copy, sort_func);
        return copy;
    }

    template <typename T>
    int
    default_compare_for_quicksort(const T& lhs, const T& rhs)
    {
        if(lhs == rhs)
        {
            return 0;
        }
        return lhs < rhs ? -1 : 1;
    }

    template <typename T>
    std::vector<T>
    get_quick_sorted(const std::vector<T>& arr)
    {
        return get_quick_sorted(arr, default_compare_for_quicksort<T>);
    }

}

