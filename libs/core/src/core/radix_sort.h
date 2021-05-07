#pragma once

#include <vector>

#include "core/cint.h"

namespace euphoria::core
{
    template <typename Int>
    struct bucket10_base
    {
        constexpr static int count = 10;

        static int
        get_index(Int i, int exp)
        {
            return (i / exp) % 10;
        }

        static int
        get_start()
        {
            return 1;
        }

        static bool
        should_keep_looping(Int max_value, int exp)
        {
            return max_value / exp > 0;
        }

        static int
        get_next_value(Int, int exp)
        {
            return exp * 10;
        }
    };

    template <typename Int>
    struct bucket16_base
    {
        constexpr static int count = 16;

        static int
        get_index(Int i, int exp)
        {
            return (i >> (exp * 4)) & 0xF;
        }

        static int
        get_start()
        {
            return 0;
        }

        static bool
        should_keep_looping(Int max_value, int exp)
        {
            if(exp == 0) { return true; }
            // -1 because we need to be sure we have passed the max value
            // todo(Gustav): use some smart bit technique to set F up to exp
            return max_value >= (1 << ((exp - 1) * 4));
        }

        static int
        get_next_value(Int, int exp)
        {
            return exp + 1;
        }
    };

    template <typename T, typename IdExtractor, typename Int>
    Int
    get_max(const std::vector<T>& arr)
    {
        Int mx = IdExtractor::get_id(arr[0]);
        for(const auto& i: arr)
        {
            Int val = IdExtractor::get_id(i);
            if(val > mx)
            {
                mx = val;
            }
        }
        return mx;
    }

    template <typename T, typename IdExtractor, typename Bucket, typename Int>
    std::vector<T>
    count_sort(const std::vector<T>& arr, int exp)
    {
        const int size = Csizet_to_int(arr.size());

        int sum[Bucket::count] = {0,};

        for(auto i = 0; i < size; i+=1)
        {
            sum[Bucket::get_index(IdExtractor::get_id(arr[i]), exp)]+=1;
        }

        for(auto i = 1; i < Bucket::count; i+=1)
        {
            sum[i] += sum[i - 1];
        }

        // cant use reserve since that will not set the size, and we cant use resize
        // or something else since that requires our object to be default
        // constructable
        // so we copy the array instead
        // todo(Gustav): look into swapping the input array instead?
        std::vector<T> ret = arr;

        for(auto i = size - 1; i >= 0; i-=1)
        {
            auto index  = Bucket::get_index(IdExtractor::get_id(arr[i]), exp);
            auto target = sum[index] - 1;
            ret[target] = arr[i];
            sum[index] -= 1;
        }

        return ret;
    }

    template <typename T, typename IdExtractor, typename Bucket, typename Int>
    void
    radix_sort(std::vector<T>* arr)
    {
        const Int max_value = get_max<T, IdExtractor, Int>(*arr);

        for(int exp = Bucket::get_start(); Bucket::should_keep_looping(max_value, exp);
            exp     = Bucket::get_next_value(max_value, exp))
        {
            *arr = count_sort<T, IdExtractor, Bucket, Int>(*arr, exp);
        }
    }

}

