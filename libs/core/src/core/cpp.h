#ifndef EUPHORIA_CPP_H
#define EUPHORIA_CPP_H

#include <string>
#include <vector>

namespace euphoria::core
{
    // todo(Gustav): provide a construct to take a iterator and return
    // iterator + index, but perhaps this header is not the best place to
    // have such construct...

    template <typename T>
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
        // todo(Gustav): fix this
        return DefaultValue<T> {}.value;
    }

    template <typename T>
    struct ArrayToPointerDecay
    {
        using Type = T;
    };

    template <typename T, std::size_t N>
    struct ArrayToPointerDecay<T[N]>
    {
        using Type = const T*;
    };

    template <typename T>
    struct CharPointerToString
    {
        using Type = T;
    };

    template <std::size_t N>
    struct CharPointerToString<char[N]>
    {
        using Type = std::string;
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
