#pragma once

#include <type_traits>

namespace eu::core
{


    template<typename T, typename I = std::underlying_type_t<T>>
    struct Bitfield
    {
        T value;

        Bitfield()
            : value(static_cast<T>(0))
        {
        }

        void
        set(T bit, bool new_value)
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
        toggle(T bit)
        {
            value = static_cast<T>(static_cast<I>(value) ^ static_cast<I>(bit));
        }

        void
        clear()
        {
            value = static_cast<T>(0);
        }

        [[nodiscard]] bool
        get(T bit) const
        {
            const I i = static_cast<I>(value) & static_cast<I>(bit);
            return bit == static_cast<T>(i);
        }

        [[nodiscard]] bool
        has_any_values() const
        {
            return value != static_cast<T>(0);
        }
    };

}
