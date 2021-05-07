#ifndef EUPHORIA_SHUFFLEBAG_H
#define EUPHORIA_SHUFFLEBAG_H

#include <vector>

#include "core/assert.h"
#include "core/random.h"

namespace euphoria::core
{
    template <typename T>
    struct ShuffleBag
    {
        [[nodiscard]] unsigned long
        GetSize() const
        {
            return data_.size();
        }

        void
        Reserve(unsigned long count)
        {
            data_.reserve(count);
        }

        void
        Add(const T& item, int amount)
        {
            ASSERT(amount > 0);

            for(int i = 0; i < amount; i++)
            {
                data_.push_back(item);
            }

            cursor_ = GetSize() - 1;
        }

        [[nodiscard]] const T&
        Next(random* rand)
        {
            ASSERT(rand);
            ASSERT(!data_.empty());  // needs data

            if(cursor_ < 1)
            {
                cursor_ = GetSize() - 1;
                return data_[0];
            }

            const auto next_position = get_next_range(rand, static_cast<int>(cursor_));

            std::swap(data_[next_position], data_[cursor_]);
            cursor_ -= 1;

            return data_[cursor_ + 1];
        }

    private:
        std::vector<T> data_;
        unsigned long  cursor_ = 0;
    };

    template<typename T>
    [[nodiscard]]
    constexpr ShuffleBag<T>
    CreateShuffleBag(const std::vector<T>& items, int amount)
    {
        auto b = ShuffleBag<T>{};
        b.Reserve(items.size() * amount);
        for(const auto& it: items)
        {
            b.Add(it, amount);
        }
        return b;
    }

}  // namespace euphoria::core

#endif  // EUPHORIA_SHUFFLEBAG_H
