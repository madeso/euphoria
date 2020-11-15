#ifndef CORE_RADIX_H
#define CORE_RADIX_H

#include <vector>

namespace euphoria::core
{
    template <typename Int>
    struct Bucket10base
    {
        enum
        {
            COUNT = 10
        };

        static int
        GetIndex(Int i, int exp)
        {
            return (i / exp) % 10;
        }

        static int
        GetStart()
        {
            return 1;
        }

        static bool
        KeepLooping(Int max_value, int exp)
        {
            return max_value / exp > 0;
        }

        static int
        NextValue(Int, int exp)
        {
            return exp * 10;
        }
    };

    template <typename Int>
    struct Bucket16base
    {
        enum
        {
            COUNT = 16
        };
        static int
        GetIndex(Int i, int exp)
        {
            return (i >> (exp * 4)) & 0xF;
        }

        static int
        GetStart()
        {
            return 0;
        }

        static bool
        KeepLooping(Int max_value, int exp)
        {
            if(exp == 0)
                return true;
            // -1 because we need to be sure we have passed the max value
            // todo: use some smart bit technique to set F up to exp
            return max_value >= (1 << ((exp - 1) * 4));
        }

        static int
        NextValue(Int, int exp)
        {
            return exp + 1;
        }
    };

    template <typename T, typename IdExtractor, typename Int>
    Int
    GetMax(const std::vector<T>& arr)
    {
        Int mx = IdExtractor::GetId(arr[0]);
        for(const auto& i: arr)
        {
            Int val = IdExtractor::GetId(i);
            if(val > mx)
            {
                mx = val;
            }
        }
        return mx;
    }

    template <typename T, typename IdExtractor, typename Bucket, typename Int>
    std::vector<T>
    CountSort(const std::vector<T>& arr, int exp)
    {
        const int size = arr.size();

        int sum[Bucket::COUNT] = {0};

        for(int i = 0; i < size; i++)
        {
            sum[Bucket::GetIndex(IdExtractor::GetId(arr[i]), exp)]++;
        }

        for(int i = 1; i < Bucket::COUNT; i++)
        {
            sum[i] += sum[i - 1];
        }

        // cant use reserve since that will not set the size, and we cant use resize
        // or something else since that requires our object to be default
        // constructable
        // so we copy the array instead
        // todo: look into swapping the input array instead?
        std::vector<T> ret = arr;

        for(int i = size - 1; i >= 0; i--)
        {
            auto index  = Bucket::GetIndex(IdExtractor::GetId(arr[i]), exp);
            auto target = sum[index] - 1;
            ret[target] = arr[i];
            sum[index]--;
        }

        return ret;
    }

    template <typename T, typename IdExtractor, typename Bucket, typename Int>
    void
    RadixSort(std::vector<T>* arr)
    {
        const Int max_value = GetMax<T, IdExtractor, Int>(*arr);

        for(int exp = Bucket::GetStart(); Bucket::KeepLooping(max_value, exp);
            exp     = Bucket::NextValue(max_value, exp))
        {
            *arr = CountSort<T, IdExtractor, Bucket, Int>(*arr, exp);
        }
    }

}  // namespace euphoria::core

#endif  // CORE_RADIX_H
