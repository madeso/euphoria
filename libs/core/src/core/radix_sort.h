#pragma once



#include "core/cint.h"

namespace euphoria::core
{
    template <typename TInt>
    struct BucketBase10
    {
        constexpr static int count = 10;

        static int
        get_index(TInt i, int exp)
        {
            return (i / exp) % 10;
        }

        static int
        get_start()
        {
            return 1;
        }

        static bool
        should_keep_looping(TInt max_value, int exp)
        {
            return max_value / exp > 0;
        }

        static int
        get_next_value(TInt, int exp)
        {
            return exp * 10;
        }
    };

    template <typename TInt>
    struct BucketBase16
    {
        constexpr static int count = 16;

        static int
        get_index(TInt i, int exp)
        {
            return (i >> (exp * 4)) & 0xF;
        }

        static int
        get_start()
        {
            return 0;
        }

        static bool
        should_keep_looping(TInt max_value, int exp)
        {
            if(exp == 0) { return true; }
            // -1 because we need to be sure we have passed the max value
            // todo(Gustav): use some smart bit technique to set F up to exp
            return max_value >= (1 << ((exp - 1) * 4));
        }

        static int
        get_next_value(TInt, int exp)
        {
            return exp + 1;
        }
    };

    template <typename T, typename TIdExtractor, typename TInt>
    TInt
    get_max(const std::vector<T>& arr)
    {
        TInt mx = TIdExtractor::get_id(arr[0]);
        for(const auto& i: arr)
        {
            TInt val = TIdExtractor::get_id(i);
            if(val > mx)
            {
                mx = val;
            }
        }
        return mx;
    }

    template <typename T, typename TIdExtractor, typename TBucket, typename TInt>
    std::vector<T>
    count_sort(const std::vector<T>& arr, int exp)
    {
        const int size = c_sizet_to_int(arr.size());

        int sum[TBucket::count] = {0,};

        for(auto arr_index = 0; arr_index < size; arr_index +=1)
        {
            sum[TBucket::get_index(TIdExtractor::get_id(arr[arr_index]), exp)]+=1;
        }

        for(auto sum_index = 1; sum_index < TBucket::count; sum_index +=1)
        {
            sum[sum_index] += sum[sum_index - 1];
        }

        // cant use reserve since that will not set the size, and we cant use resize
        // or something else since that requires our object to be default
        // constructable
        // so we copy the array instead
        // todo(Gustav): look into swapping the input array instead?
        std::vector<T> ret = arr;

        for(auto arr_index = size - 1; arr_index >= 0; arr_index -=1)
        {
            auto sum_index = TBucket::get_index(TIdExtractor::get_id(arr[arr_index]), exp);
            auto target = sum[sum_index] - 1;
            ret[target] = arr[arr_index];
            sum[sum_index] -= 1;
        }

        return ret;
    }

    // todo(Gustav): rename to do_inplace_radix_sort
    template <typename T, typename TIdExtractor, typename TBucket, typename TInt>
    void
    do_inplace_radix_sort(std::vector<T>* arr)
    {
        const TInt max_value = get_max<T, TIdExtractor, TInt>(*arr);

        for(int exp = TBucket::get_start(); TBucket::should_keep_looping(max_value, exp);
            exp = TBucket::get_next_value(max_value, exp))
        {
            *arr = count_sort<T, TIdExtractor, TBucket, TInt>(*arr, exp);
        }
    }

}

