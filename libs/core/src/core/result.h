#pragma once


#include <optional>
#include <string>

namespace euphoria::core
{
    template <typename T>
    struct result
    {
        using self = result<T>;

        operator bool() const
        {
            return value.has_value();
        }


        [[nodiscard]]
        const T&
        operator *() const
        {
            return *value;
        }

        [[nodiscard]]
        const T&
        get_value() const
        {
            return *value;
        }

        [[nodiscard]]
        const std::string&
        get_error() const
        {
            return *error;
        }

        [[nodiscard]]
        static
        self
        create_value(const T& t)
        {
            return self{t, std::nullopt};
        }

        [[nodiscard]]
        static
        self
        create_error(const std::string& error = "unknown error")
        {
            return self{std::nullopt, error};
        }

    private:
        std::optional<T> value;
        std::optional<std::string> error;

        result
        (
            const std::optional<T>& v,
            const std::optional<std::string>& e
        )
        : value(v)
        , error(e)
        {
        }
    };
}

