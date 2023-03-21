#pragma once


#include <cstddef>

namespace euphoria::core
{
    // todo(Gustav): rename to do_inplace_insertion_sort
    template <typename T, typename TSortFunc>
    void
    insertion_sort(std::vector<T>* pointer_to_array, TSortFunc sort_func)
    {
        auto& array = *pointer_to_array;
        const auto length = array.size();

        size_t i = 1;
        while(i < length)
        {
            size_t j = i;
            while(j > 0 && sort_func(array[j - 1], array[j]) > 0)
            {
                std::swap(array[j], array[j - 1]);
                j = j - 1;
            }
            i = i + 1;
        }
    }

    // todo(Gustav): rename to get_insertion_sorted
    template <typename T, typename TSortFunc>
    std::vector<T>
    insertion_sort(const std::vector<T>& array, TSortFunc sort_func)
    {
        auto copy = array;
        insertion_sort(&copy, sort_func);
        return copy;
    }

    // rename to default_compare
    template <typename T>
    int
    default_insertion_sort(const T& lhs, const T& rhs)
    {
        if(lhs == rhs)
        {
            return 0;
        }
        return lhs < rhs ? -1 : 1;
    }

    // todo(Gustav): rename to get_insertion_sorted
    template <typename T>
    std::vector<T>
    insertion_sort(const std::vector<T>& array)
    {
        return insertion_sort(array, default_insertion_sort<T>);
    }

}

