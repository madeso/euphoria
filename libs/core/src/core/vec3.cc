#include "core/vec3.h"


namespace euphoria::core
{
    float*
    Scale3f::get_data_ptr()
    {
        return &x;
    }


    [[nodiscard]] const float*
    Scale3f::get_data_ptr() const
    {
        return &x;
    }


    Scale3f::Scale3f(float a)
        : x(a)
        , y(a)
        , z(a)
    {
    }


    Scale3f::Scale3f(const std::tuple<float, float, float>& a)
        : x(std::get<0>(a))
        , y(std::get<1>(a))
        , z(std::get<2>(a))
    {
    }


    Scale3f::Scale3f(float ax, float ay, float az)
        : x(ax)
        , y(ay)
        , z(az)
    {
    }


    Scale3f::Scale3f(const float* a)
        : x(a[0])
        , y(a[1])
        , z(a[2])
    {
    }


    ////////////////////////////////////////////////////////////////////////////////
    

    float*
    vec3f::get_data_ptr()
    {
        return &x;
    }


    [[nodiscard]] const float*
    vec3f::get_data_ptr() const
    {
        return &x;
    }


    vec3f::vec3f(float a)
        : x(a)
        , y(a)
        , z(a)
    {
    }


    vec3f::vec3f(const std::tuple<float, float, float>& a)
        : x(std::get<0>(a))
        , y(std::get<1>(a))
        , z(std::get<2>(a))
    {
    }


    vec3f::vec3f(const float* a)
        : x(a[0])
        , y(a[1])
        , z(a[2])
    {
    }


    vec3f::vec3f(const vec2f& a, float az)
        : x(a.x)
        , y(a.y)
        , z(az)
    {
    }


    void
    vec3f::operator+=(const vec3f& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
    }


    void
    vec3f::operator-=(const vec3f& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
    }


    vec3f
    vec3f::operator-() const
    {
        return vec3f(-this->x, -this->y, -this->z);
    }


    vec3f
    vec3f::from_to(const vec3f& from, const vec3f& to)
    {
        return vec3f{to.x - from.x, to.y - from.y, to.z - from.z};
    }


    void
    vec3f::operator/=(float rhs)
    {
        x /= rhs;
        y /= rhs;
        z /= rhs;
    }


    void
    vec3f::operator*=(float rhs)
    {
        x *= rhs;
        y *= rhs;
        z *= rhs;
    }


    [[nodiscard]] float
    vec3f::get_length() const
    {
        return sqrt(get_length_squared());
    }


    void
    vec3f::normalize()
    {
        const float l2 = get_length_squared();
        if(is_equal(l2, 0.0f))
        {
            *this = common::up;
        }
        else
        {
            *this /= sqrt(l2);
        }
    }


    [[nodiscard]] unit3f
    vec3f::get_normalized() const
    {
        vec3f r = *this;
        r.normalize();
        return unit3f::to_unit(r);
    }

    [[nodiscard]] unit3f
    vec3f::as_normalized() const
    {
        return unit3f::to_unit(*this);
    }


    ////////////////////////////////////////////////////////////////////////////////


    unit3f
    unit3f::to_unit(float x, float y, float z)
    {
        return unit3f{x, y, z};
    }


    unit3f
    unit3f::to_unit(const vec3f& v)
    {
        return unit3f{v.x, v.y, v.z};
    }


    ////////////////////////////////////////////////////////////////////////////////
    /// Math operators


    vec3f
    operator+(const vec3f& lhs, const vec3f& rhs)
    {
        vec3f r = lhs;
        r += rhs;
        return r;
    }


    vec3f
    operator-(const vec3f& lhs, const vec3f& rhs)
    {
        vec3f r = lhs;
        r -= rhs;
        return r;
    }


    vec3f operator*(float lhs, const vec3f& rhs)
    {
        vec3f r = rhs;
        r *= lhs;
        return r;
    }


    vec3f operator*(const vec3f& lhs, float rhs)
    {
        vec3f r = lhs;
        r *= rhs;
        return r;
    }


    vec3f
    operator/(const vec3f& lhs, float rhs)
    {
        vec3f r = lhs;
        r /= rhs;
        return r;
    }


    vec3f
    operator/(float lhs, const vec3f& rhs)
    {
        const vec3f r {lhs / rhs.x, lhs / rhs.y, lhs / rhs.z};
        return r;
    }


    ////////////////////////////////////////////////////////////////////////////////
    /// Functions


    vec3f lerp_vec3f(const vec3f& f, float v, const vec3f& t)
    {
        return vec3f
        {
            lerp_float(f.x, v, t.x),
            lerp_float(f.y, v, t.y),
            lerp_float(f.z, v, t.z)
        };
    }


    float
    vec3f::dot(const vec3f& rhs) const
    {
        return x * rhs.x + y * rhs.y + z * rhs.z;
    }


    vec3f
    vec3f::cross(const vec3f& u) const
    {
        return vec3f(
                (y * u.z) - (z * u.y),
                (z * u.x) - (x * u.z),
                (x * u.y) - (y * u.x));
    }


    ////////////////////////////////////////////////////////////////////////////////
    /// Transformations


    // todo(Gustav): implement transformations


    ////////////////////////////////////////////////////////////////////////////////
    /// Printing


    std::ostream&
    operator<<(std::ostream& stream, const vec3f& v)
    {
        return stream << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    }


    std::ostream&
    operator<<(std::ostream& stream, const unit3f& v)
    {
        return stream << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    }


    std::ostream&
    operator<<(std::ostream& stream, const Scale3f& v)
    {
        return stream << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    }


}
