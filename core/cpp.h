#ifndef EUPHORIA_CPP_H
#define EUPHORIA_CPP_H

#include <string>


namespace euphoria::core
{
    template <class T>
    void
    NotUsed(T const&)
    {}

    template <typename T>
    struct DefaultValue
    {
        T value;
    };

    template <typename T>
    constexpr T
    Default()
    {
        // todo: fix this
        return DefaultValue<T> {}.value;
    }

    template <typename T>
    struct ArrayToPointerDecay
    {
        typedef T Type;
    };

    template <typename T, std::size_t N>
    struct ArrayToPointerDecay<T[N]>
    {
        typedef const T* Type;
    };

    template <typename T>
    struct CharPointerToString
    {
        typedef T Type;
    };

    template <std::size_t N>
    struct CharPointerToString<char[N]>
    {
        typedef std::string Type;
    };

    template <typename T>
    struct IsVector
    {
        static constexpr bool value = false;
    };

    template <typename T>
    struct IsVector<std::vector<T>>
    {
        static constexpr bool value = true;
    };

}  // namespace euphoria::core

#endif  // EUPHORIA_CPP_H
