#pragma once

#include "assert/assert.h"


namespace eu::core
{
    // todo(Gustav): consider removing or simplifying this, it looks confusing and complex
    template<typename T>
    struct StepIterator
    {
        T current;
        T to;
        T step;
        bool ended;
        bool last;

        StepIterator(T c, T t, T s, bool e)
            : current(c), to(t), step(s), ended(e), last(is_ended(c, t, s))
        {
        }

        bool operator!=(const StepIterator<T>& rhs) const
        {
            ASSERT(to == rhs.to);
            ASSERT(step == rhs.step);

            return ended != rhs.ended;
        }

        T operator*() const
        {
            return current;
        }

        void operator++()
        {
            current += step;
            const auto curr = is_ended(current, to, step);
            if(curr != last)
            {
                ended = true;
            }
        }

        static bool is_ended(T c, T t, T s)
        {
            if(s > 0) { return c >= t;}
            else      { return c > t;}
        }
    };


    template<typename T>
    struct StepIteratorCreator
    {
        T from;
        T to;
        T step;

        StepIterator<T> begin() const
        {
            return StepIterator<T>{from, to, step, false};
        }

        StepIterator<T> end() const
        {
            return StepIterator<T>{from, to, step, true};
        }
    };

    template<typename T>
    StepIteratorCreator<T> iterate(const T from, T to, T step=1)
    {
        return StepIteratorCreator<T>{from, to, step};
    }
}

