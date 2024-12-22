#include "base/vec3.h"

namespace eu
{

    ////////////////////////////////////////////////////////////////////////////////

    float *
    v3::get_data_ptr()
    {
        return &x;
    }

    [[nodiscard]] const float *
    v3::get_data_ptr() const
    {
        return &x;
    }

    v3::v3(float a)
        : x(a), y(a), z(a)
    {
    }

    v3::v3(const std::tuple<float, float, float> &a)
        : x(std::get<0>(a)), y(std::get<1>(a)), z(std::get<2>(a))
    {
    }

    v3::v3(const float *a)
        : x(a[0]), y(a[1]), z(a[2])
    {
    }

    void
    v3::operator+=(const v3 &rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
    }

    void
    v3::operator-=(const v3 &rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
    }

    v3
    v3::operator-() const
    {
        return {-this->x, -this->y, -this->z};
    }

    v3
    v3::from_to(const v3 &from, const v3 &to)
    {
        return {to.x - from.x, to.y - from.y, to.z - from.z};
    }

    void
    v3::operator/=(float rhs)
    {
        x /= rhs;
        y /= rhs;
        z /= rhs;
    }

    void
    v3::operator*=(float rhs)
    {
        x *= rhs;
        y *= rhs;
        z *= rhs;
    }

    [[nodiscard]] float
    v3::get_length() const
    {
        return sqrt(get_length_squared());
    }

    void
    v3::normalize()
    {
        const float l2 = get_length_squared();
        if (is_equal(l2, 0.0f))
        {
            *this = common::up;
        }
        else
        {
            *this /= sqrt(l2);
        }
    }

    [[nodiscard]] n3
    v3::get_normalized() const
    {
        v3 r = *this;
        r.normalize();
        return n3{r};
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// Math operators

    v3
    operator+(const v3 &lhs, const v3 &rhs)
    {
        v3 r = lhs;
        r += rhs;
        return r;
    }

    v3
    operator-(const v3 &lhs, const v3 &rhs)
    {
        v3 r = lhs;
        r -= rhs;
        return r;
    }

    v3 operator*(float lhs, const v3 &rhs)
    {
        v3 r = rhs;
        r *= lhs;
        return r;
    }

    v3 operator*(const v3 &lhs, float rhs)
    {
        v3 r = lhs;
        r *= rhs;
        return r;
    }

    v3
    operator/(const v3 &lhs, float rhs)
    {
        v3 r = lhs;
        r /= rhs;
        return r;
    }

    v3
    operator/(float lhs, const v3 &rhs)
    {
        const v3 r{lhs / rhs.x, lhs / rhs.y, lhs / rhs.z};
        return r;
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// Functions

    v3 lerp_v3(const v3 &f, float v, const v3 &t)
    {
        return v3{
            lerp_float(f.x, v, t.x),
            lerp_float(f.y, v, t.y),
            lerp_float(f.z, v, t.z)};
    }

    float
    v3::dot(const v3 &rhs) const
    {
        return x * rhs.x + y * rhs.y + z * rhs.z;
    }

    v3
    v3::cross(const v3 &u) const
    {
        return {
            (y * u.z) - (z * u.y),
            (z * u.x) - (x * u.z),
            (x * u.y) - (y * u.x)};
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// Transformations

    // todo(Gustav): implement transformations

    ////////////////////////////////////////////////////////////////////////////////
    /// Printing

    std::string string_from(const v3 &v) { return fmt::format("({}, {}, {})", v.x, v.y, v.z); }
    std::string string_from(const n3 &v) { return fmt::format("({}, {}, {})", v.x, v.y, v.z); }
}
