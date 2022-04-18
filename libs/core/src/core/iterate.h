#pragma once

namespace euphoria::core
{
    template<typename T>
    struct StepIterator
    {
        T current; T to; T step;
        bool ended;

        bool
        operator!=(const StepIterator<T>& rhs) const
        {
            if(ended != rhs.ended ) return true;
            if(to != rhs.to ) return true;
            if(step != rhs.step ) return true;
            return false;
        }

        T
        operator*() const
        {
            return current;
        }

        void
        operator++()
        {
            current += step;
            ended = current >= to;
        }
    };


    template<typename T>
    struct StepIteratorCreator
    {
        T from;
        T to;
        T step;

        StepIterator<T>
        begin() const
        {
            return StepIterator<T>{from, to, step, false};
        }

        StepIterator<T>
        end() const
        {
            return StepIterator<T>{from, to, step, true};
        }
    };


    template<typename T>
    StepIteratorCreator<T>
    iterate(const T from, T to, T step=1)
    {
        return StepIteratorCreator<T>{from, to, step};
    }
}

