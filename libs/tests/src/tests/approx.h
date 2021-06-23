#pragma once

#include <cmath>
#include <sstream>
#include <limits>

#include "catch.hpp"


namespace euphoria::tests
{
    struct approx_data
    {
        float epsilon;
        float scale;
        float margin;
    };

    // https://groups.google.com/forum/#!topic/catch-forum/03HhdvUzIhg

    template <typename T>
    bool
    approximately_equal(const T& lhs, const T& rhs, const approx_data& data);


    template <>
    bool
    approximately_equal(
            float const& lhs,
            float const& rhs,
            const approx_data& data);


    ////////////////////////////////////////////////////////////////////////////////
    // modified Catch Approx

    namespace custom
    {
        template <typename T>
        struct approx
        {
        public:
            explicit approx(T value)
                : epsilon(std::numeric_limits<float>::epsilon() * 100)
                , margin(0.0)
                , scale(1.0)
                , value(value)
            {}

            friend bool
            operator==(T lhs, approx<T> const& rhs)
            {
                auto data = approx_data{};
                data.epsilon = rhs.epsilon;
                data.scale = rhs.scale;
                data.margin = rhs.margin;
                return approximately_equal(lhs, rhs.value, data);
            }

            friend bool
            operator==(approx<T> const& lhs, T rhs)
            {
                return operator==(rhs, lhs);
            }

            friend bool
            operator!=(T lhs, approx<T> const& rhs)
            {
                return !operator==(lhs, rhs);
            }

            friend bool
            operator!=(approx<T> const& lhs, T rhs)
            {
                return !operator==(rhs, lhs);
            }

            approx&
            set_epsilon(float new_epsilon)
            {
                epsilon = new_epsilon;
                return *this;
            }

            approx&
            set_margin(float new_margin)
            {
                margin = new_margin;
                return *this;
            }

            approx&
            set_scale(float new_scale)
            {
                scale = new_scale;
                return *this;
            }

            [[nodiscard]] std::string
            to_string() const
            {
                Catch::ReusableStringStream rss;
                rss << "Approx( " << ::Catch::Detail::stringify(value)
                    << " )";
                return rss.str();
            }

        private:
            float epsilon;
            float margin;
            float scale;
            T value;
        };
    }

    template <typename T>
    std::ostream&
    operator<<(std::ostream& stream, const custom::approx<T>& v)
    {
        return stream << v.to_string();
    }

    ////////////////////////////////////////////////////////////////////////////////


    template <typename T>
    custom::approx<T>
    approx(T const& t)
    {
        return custom::approx<T>(t);
    }
}
