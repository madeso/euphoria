#ifndef EUPHORIA_CORE_HISTORY_H
#define EUPHORIA_CORE_HISTORY_H

#include <vector>

#include "core/assert.h"

namespace euphoria::core
{
    template <typename T>
    struct History
    {
        explicit History(std::size_t s) : max(s) {}

        void
        Push(const T& t)
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
            return d.size();
        }

        std::vector<T> d;
        std::size_t    max;
    };
}

#endif  // EUPHORIA_CORE_HISTORY_H
