#ifndef EUPHORIA_CORE_HISTORY_H
#define EUPHORIA_CORE_HISTORY_H

#include <vecotr>

namespace euphoria::core
{
    template <typename T>
    struct History
    {
        explicit History(int s) : max(s) {}

        void
        Push(const T& t)
        {
            if(d.size() == max)
            {
                d.erase(d.begin());
            }
            d.push_back(t);
        }

        const T*
        data() const
        {
            assert(!d.empty());
            return &d[0];
        }

        int
        size() const
        {
            return d.size();
        }

        std::vector<T> d;
        int            max;
    };
}  // namespace euphoria::core

#endif  // EUPHORIA_CORE_HISTORY_H
