#pragma once

#include <vector>

namespace euphoria::core
{
    // todo(Gustav) replace with range
    template<typename T>
    struct subvec
    {
        using it = typename std::vector<T>::iterator;

        explicit subvec(std::vector<T>* s)
            : source(s)
            , ibegin(0)
            , iend(s->size())
        {
        }

        subvec(std::vector<T>* s, size_t b, size_t e)
            : source(s)
            , ibegin(b)
            , iend(e)
        {
        }

        std::vector<T>* source;
        size_t ibegin;
        size_t iend;

        it
        begin() const
        {
            return source->begin() + ibegin;
        }

        it
        end() const
        {
            return source->begin() + iend;
        }

        [[nodiscard]] size_t
        size() const
        {
            return iend - ibegin;
        }

        [[nodiscard]] bool
        empty() const
        {
            return ibegin >= iend;
        }

        T&
        operator[](int index)
        {
            return (*source)[ibegin+index];
        }

        const T&
        operator[](int index) const
        {
            return (*source)[ibegin+index];
        }

        subvec<T>
        sub(size_t start, size_t end)
        {
            return {source, ibegin+start, ibegin+end};
        }
    };
}
