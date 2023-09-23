#pragma once


#include <optional>


namespace eu
{
    template <typename T, typename Error = std::string>
    struct Result
    {
        using Self = Result<T, Error>;

        [[nodiscard]]
        static Self create_value(const T& t)
        {
            return Self{t, std::nullopt};
        }

        [[nodiscard]]
        static Self create_error(const Error& error)
        {
            // argparse is assuming a empty string here...
            // todo(Gustav): fix so we always set unknown error and let all argparse functions explcitly parse a empty string
            // or just don't have a empty error message
            return Self{std::nullopt, error};
        }

        [[nodiscard]]
        const T& get_value() const
        {
            return *value;
        }

        [[nodiscard]]
        const Error& get_error() const
        {
            return *error;
        }

        operator bool() const
        {
            return value.has_value();
        }


        [[nodiscard]]
        const T& operator *() const
        {
            return *value;
        }
    private:
        std::optional<T> value;
        std::optional<Error> error;

        Result
        (
            const std::optional<T>& v,
            const std::optional<Error>& e
        )
        : value(v)
        , error(e)
        {
        }
    };
}

