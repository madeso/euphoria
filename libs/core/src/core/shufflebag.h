#pragma once


#include <vector>

#include "core/assert.h"
#include "core/random.h"

namespace euphoria::core
{
    template <typename T>
    struct shufflebag
    {
        [[nodiscard]] unsigned long
        get_size() const
        {
            return data_.size();
        }

        void
        reserve(unsigned long count)
        {
            data_.reserve(count);
        }

        void
        add(const T& item, int amount)
        {
            ASSERT(amount > 0);

            for(int i = 0; i < amount; i++)
            {
                data_.push_back(item);
            }

            cursor_ = get_size() - 1;
        }

        [[nodiscard]] const T&
        get_random_item(random* rand)
        {
            ASSERT(rand);
            ASSERT(!data_.empty());  // needs data

            if(cursor_ < 1)
            {
                cursor_ = get_size() - 1;
                return data_[0];
            }

            const auto next_position = get_random_in_range(rand, static_cast<int>(cursor_));

            std::swap(data_[next_position], data_[cursor_]);
            cursor_ -= 1;

            return data_[cursor_ + 1];
        }

    private:
        std::vector<T> data_;
        unsigned long cursor_ = 0;
    };

    template<typename T>
    [[nodiscard]]
    constexpr shufflebag<T>
    create_shuffle_bag(const std::vector<T>& items, int amount)
    {
        auto b = shufflebag<T>{};
        b.reserve(items.size() * amount);
        for(const auto& it: items)
        {
            b.add(it, amount);
        }
        return b;
    }

}

