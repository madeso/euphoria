#ifndef EUPHORIA_CORE_BITFIELD_H
#define EUPHORIA_CORE_BITFIELD_H

#include <type_traits>

namespace euphoria::core
{

    template<typename T, typename I = std::underlying_type_t<T>>
    struct Bitfield
    {
        Bitfield()
        : value(static_cast<T>(0))
        {}

        bool
        HasAnyValues() const
        {
            return value != static_cast<T>(0);
        }

        void
        Set(T bit, bool new_value)
        {
            if(new_value)
            {
                value = static_cast<T>(static_cast<I>(value) | static_cast<I>(bit));
            }
            else
            {
                value = static_cast<T>(static_cast<I>(value) & ~static_cast<I>(bit));
            }
        }

        void
        Toggle(T bit)
        {
            value = static_cast<T>(static_cast<I>(value) ^ static_cast<I>(bit));
        }

        void
        Clear()
        {
            value = static_cast<T>(0);
        }

        bool
        Get(T bit) const
        {
            const I i = static_cast<I>(value) & static_cast<I>(bit);
            return bit == static_cast<T>(i);
        }

        T value;
    };

}

#endif  // EUPHORIA_CORE_BITFIELD_H