#pragma once


#include <optional>


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
        Self
        create_value(const T& t)
        {
            return Self{t, std::nullopt};
        }

        [[nodiscard]]
        static
        Self
        create_error(const std::string& error = "")
        {
            // argparse is assuming a empty string here...
            // todo(Gustav): fix so we always set unknown error and let all argparse functions explcitly parse a empty string
            // or just don't have a empty error message
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

