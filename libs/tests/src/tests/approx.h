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
        struct Approx
        {
        public:
            explicit Approx(T value)
                : m_epsilon(std::numeric_limits<float>::epsilon() * 100)
                , m_margin(0.0)
                , m_scale(1.0)
                , m_value(value)
            {}

            friend bool
            operator==(T lhs, Approx<T> const& rhs)
            {
                approx_data data;
                data.epsilon = rhs.m_epsilon;
                data.scale = rhs.m_scale;
                data.margin = rhs.m_margin;
                return approximately_equal(lhs, rhs.m_value, data);
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

            Approx&
            epsilon(float newEpsilon)
            {
                m_epsilon = newEpsilon;
                return *this;
            }

            Approx&
            margin(float newMargin)
            {
                m_margin = newMargin;
                return *this;
            }

            Approx&
            scale(float newScale)
            {
                m_scale = newScale;
                return *this;
            }

            std::string
            toString() const
            {
                Catch::ReusableStringStream rss;
                rss << "Approx( " << ::Catch::Detail::stringify(m_value)
                    << " )";
                return rss.str();
            }

        private:
            float m_epsilon;
            float m_margin;
            float m_scale;
            T m_value;
        };
    }

    template <typename T>
    std::ostream&
    operator<<(std::ostream& stream, const custom::Approx<T>& v)
    {
        return stream << v.toString();
    }

    ////////////////////////////////////////////////////////////////////////////////


    template <typename T>
    custom::Approx<T>
    approx(T const& t)
    {
        return custom::Approx<T>(t);
    }
}
