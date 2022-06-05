#pragma once



namespace euphoria::core
{
    // todo(Gustav) replace with range
    template<typename T>
    struct SubVec
    {
        using Iterator = typename std::vector<T>::iterator;

        explicit SubVec(std::vector<T>* s)
            : source(s)
            , ibegin(0)
            , iend(s->size())
        {
        }

        SubVec(std::vector<T>* s, std::size_t b, std::size_t e)
            : source(s)
            , ibegin(b)
            , iend(e)
        {
        }

        std::vector<T>* source;
        std::size_t ibegin;
        std::size_t iend;

        Iterator
        begin() const
        {
            return source->begin() + ibegin;
        }

        Iterator
        end() const
        {
            return source->begin() + iend;
        }

        [[nodiscard]] std::size_t
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

        SubVec<T>
        sub(std::size_t start, std::size_t end)
        {
            return {source, ibegin+start, ibegin+end};
        }
    };
}
