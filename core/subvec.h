#ifndef EUPHORIA_CORE_SUBVEC_H
#define EUPHORIA_CORE_SUBVEC_H

#include <vector>

namespace euphoria::core
{
    template<typename T>
    struct SubVec
    {
        using it = typename std::vector<T>::iterator;

        explicit SubVec(std::vector<T>* s)
            : source(s)
            , ibegin(0)
            , iend(s->size())
        {
        }

        SubVec(std::vector<T>* s, size_t b, size_t e)
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

        size_t
        size() const
        {
            return iend - ibegin;
        }

        bool
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

        SubVec<T>
        Sub(size_t start, size_t end)
        {
            return {source, ibegin+start, ibegin+end};
        }
    };
}

#endif  // EUPHORIA_CORE_SUBVEC_H
