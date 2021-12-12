#pragma once

#include <vector>

#include "core/assert.h"
#include "core/cint.h"

namespace euphoria::core
{
    // stores the "last N" values of some type, where N is configurable
    template <typename T>
    struct LastN
    {
        explicit LastN(std::size_t s) : max(s) {}

        void
        push(const T& t)
        {
            if(d.size() == max)
            {
                d.erase(d.begin());
            }
            d.push_back(t);
        }

        [[nodiscard]] const T*
        data() const
        {
            ASSERT(!d.empty());
            return &d[0];
        }

        [[nodiscard]] int
        size() const
        {
            return c_sizet_to_int(d.size());
        }

        std::vector<T> d;
        std::size_t max;
    };
}

