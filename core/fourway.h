#ifndef EUPHORIA_CORE_FOURWAY_H
#define EUPHORIA_CORE_FOURWAY_H

namespace euphoria::core
{
    template<typename T>
    struct Fourway
    {
        T left;
        T right;
        T up;
        T down;

        explicit Fourway(const T& t)
            : left(t)
            , right(t)
            , up(t)
            , down(t)
        {
        }

        Fourway
        (
            const T& horizontal,
            const T& vertical
        )
            : left(horizontal)
            , right(horizontal)
            , up(vertical)
            , down(vertical)
        {
        }

        Fourway
        (
            const T& l,
            const T& r,
            const T& u,
            const T& d
        )
            : left(l)
            , right(r)
            , up(u)
            , down(d)
        {
        }
    };
}

#endif  // EUPHORIA_CORE_FOURWAY_H
