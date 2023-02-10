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


    [[nodiscard]] float
    Scale3f::get_component_sum() const
    {
        return x + y + z;
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


    [[nodiscard]] float
    vec3f::get_component_sum() const
    {
        return x + y + z;
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


    vec3f::vec3f(float ax, float ay, float az)
        : x(ax)
        , y(ay)
        , z(az)
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


    float
    vec3f::get_length_squared() const
    {
        return x * x + y * y + z * z;
    }


    vec3f
    vec3f::from_to(const vec3f& from, const vec3f& to)
    {
        return vec3f{to.x - from.x, to.y - from.y, to.z - from.z};
    }


    vec3f
    vec3f::zero()
    {
        return vec3f{0.0f, 0.0f, 0.0f};
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
        if(is_equal(l2, c_int_to_t<float>(0)))
        {
            *this = unit3f::up();
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
    unit3f::operator-() const
    {
        return unit3f(-this->x, -this->y, -this->z);
    }


    [[nodiscard]] bool
    unit3f::is_valid() const
    {
        return is_equal(this->get_length_squared(), c_int_to_t<float>(1));
    }


    unit3f
    unit3f::x_axis()
    {
        return unit3f {1, 0, 0};
    }


    unit3f
    unit3f::y_axis()
    {
        return unit3f {0, 1, 0};
    }


    unit3f
    unit3f::z_axis()
    {
        return unit3f(0, 0, 1);
    }


    unit3f
    unit3f::up()
    {
        return y_axis();
    }


    unit3f
    unit3f::down()
    {
        return -y_axis();
    }


    unit3f
    unit3f::right()
    {
        return x_axis();
    }


    unit3f
    unit3f::left()
    {
        return -x_axis();
    }


    unit3f
    unit3f::in()
    {
        return -z_axis();
    }


    unit3f
    unit3f::out()
    {
        return z_axis();
    }


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


    unit3f::unit3f(float a, float b, float c)
        : vec3f(a, b, c)
    {
        ASSERT(is_valid());
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


    vec3f
    component_multiply(const vec3f& lhs, const vec3f& rhs)
    {
        return vec3f(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
    }


    ////////////////////////////////////////////////////////////////////////////////
    /// Functions


    float
    dot(const vec3f& lhs, const vec3f& rhs)
    {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
    }


    vec3f
    cross(const vec3f& v, const vec3f& u)
    {
        return vec3f(
                (v.y * u.z) - (v.z * u.y),
                (v.z * u.x) - (v.x * u.z),
                (v.x * u.y) - (v.y * u.x));
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
