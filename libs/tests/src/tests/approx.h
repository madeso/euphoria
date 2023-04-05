#pragma once

#include <cmath>

#include <limits>

#include "catch.hpp"


namespace euphoria::tests
{
    struct ApproxData
    {
        float epsilon;
        float scale;
        float margin;
    };

    // https://groups.google.com/forum/#!topic/catch-forum/03HhdvUzIhg

    template <typename T>
    bool
    is_approximately_equal(const T& lhs, const T& rhs, const ApproxData& data);


    template <>
    bool
    is_approximately_equal(
            float const& lhs,
            float const& rhs,
            const ApproxData& data);


    ////////////////////////////////////////////////////////////////////////////////
    // modified Catch Approx

    namespace custom
    {
        template <typename T>
        struct Approx
        {
        public:
            explicit Approx(T v)
                : epsilon{std::numeric_limits<float>::epsilon() * 100}
                , value{v}
            {
            }

            Approx&
            set_epsilon(float new_epsilon)
            {
                epsilon = new_epsilon;
                return *this;
            }

            Approx&
            set_margin(float new_margin)
            {
                margin = new_margin;
                return *this;
            }

            Approx&
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

            friend bool
            operator==(T lhs, Approx<T> const& rhs)
            {
                auto data = ApproxData{};
                data.epsilon = rhs.epsilon;
                data.scale = rhs.scale;
                data.margin = rhs.margin;
                return is_approximately_equal(lhs, rhs.value, data);
            }

            friend bool
            operator==(Approx<T> const& lhs, T rhs)
            {
                return operator==(rhs, lhs);
            }

            friend bool
            operator!=(T lhs, Approx<T> const& rhs)
            {
                return !operator==(lhs, rhs);
            }

            friend bool
            operator!=(Approx<T> const& lhs, T rhs)
            {
                return !operator==(rhs, lhs);
            }

        private:
            float epsilon;
            float margin = 0.0f;
            float scale = 1.0f;

            T value;
        };
    }

    template <typename T>
    std::ostream&
    operator<<(std::ostream& stream, const custom::Approx<T>& v)
    {
        return stream << v.to_string();
    }

    ////////////////////////////////////////////////////////////////////////////////


    template <typename T>
    custom::Approx<T>
    approx(T const& t)
    {
        return custom::Approx<T>(t);
    }
}
