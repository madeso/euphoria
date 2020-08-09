#ifndef EUPHORIA_CORE_RESULT_H
#define EUPHORIA_CORE_RESULT_H

#include <optional>
#include <string>

namespace euphoria::core
{
    template <typename T>
    struct Result
    {
        using Self = Result<T>;

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
        Value() const
        {
            return *value;
        }

        [[nodiscard]]
        const std::string&
        Error() const
        {
            return *error;
        }

        [[nodiscard]]
        static
        Self
        True(const T& t)
        {
            return Self{t, std::nullopt};
        }

        [[nodiscard]]
        static
        Self
        False(const std::string& error = "")
        {
            return Self{std::nullopt, error};
        }

    private:
        std::optional<T> value;
        std::optional<std::string> error;

        Result
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

#endif  // EUPHORIA_CORE_RESULT_H

