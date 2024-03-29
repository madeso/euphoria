#pragma once



#include "range/v3/view/span.hpp"

#include "assert/assert.h"
#include "base/random.h"
#include "base/cint.h"

namespace eu::core
{
    template <typename T>
    struct Shufflebag
    {
        Shufflebag() = default;

        void reserve(int count)
        {
            data.reserve(c_int_to_sizet(count));
        }

        void add(const T& item, int amount)
        {
            ASSERT(amount > 0);

            for(int step = 0; step < amount; step++)
            {
                data.push_back(item);
            }

            cursor = get_size() - 1;
        }

        [[nodiscard]] const T& get_random_item(Random* rand)
        {
            ASSERT(rand);
            ASSERT(!data.empty()); // needs data

            if(cursor < 1)
            {
                cursor = get_size() - 1;
                return data[0];
            }

            const auto next_position = get_random_in_range(rand, static_cast<int>(cursor));

            std::swap(data[next_position], data[cursor]);
            cursor -= 1;

            return data[cursor + 1];
        }

        [[nodiscard]] int get_size() const
        {
            return c_sizet_to_int(data.size());
        }

    private:
        std::vector<T> data;
        unsigned long cursor = 0;
    };


    template<typename T>
    [[nodiscard]]
    constexpr Shufflebag<T>
    create_shuffle_bag(const ranges::span<const T>& items, int amount)
    {
        auto b = Shufflebag<T>{};
        b.reserve(c_sizet_to_int(items.size()) * amount);
        for(const auto& it: items)
        {
            b.add(it, amount);
        }
        return b;
    }


    template<typename T>
    [[nodiscard]]
    constexpr Shufflebag<T>
    create_shuffle_bag(const std::vector<T>& items, int amount)
    {
        return create_shuffle_bag(ranges::span<T>{items.begin(), items.end()}, amount);
    }

}

