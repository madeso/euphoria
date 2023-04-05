#pragma once


#include <cstddef>

namespace euphoria::core
{
    template <typename T, typename TSortFunc>
    void
    do_inplace_insertion_sort(std::vector<T>* pointer_to_array, TSortFunc sort_func)
    {
        auto& array = *pointer_to_array;
        const auto length = array.size();

        size_t index = 1;
        while(index < length)
        {
            size_t j = index;
            while(j > 0 && sort_func(array[j - 1], array[j]) > 0)
            {
                std::swap(array[j], array[j - 1]);
                j = j - 1;
            }
            index = index + 1;
        }
    }

    template <typename T, typename TSortFunc>
    std::vector<T>
    get_insertion_sorted(const std::vector<T>& array, TSortFunc sort_func)
    {
        auto copy = array;
        do_inplace_insertion_sort(&copy, sort_func);
        return copy;
    }

    template <typename T>
    int
    default_compare_for_insertion_sort(const T& lhs, const T& rhs)
    {
        if(lhs == rhs)
        {
            return 0;
        }
        return lhs < rhs ? -1 : 1;
    }

    template <typename T>
    std::vector<T>
    get_insertion_sorted(const std::vector<T>& array)
    {
        return get_insertion_sorted(array, default_compare_for_insertion_sort<T>);
    }

}

