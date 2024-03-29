#include "base/vec2.h"

#include "base/random.h"
#include "base/angle.h"



namespace eu
{
    vec2f::vec2f(const std::tuple<float, float>& a)
        : x(std::get<0>(a))
        , y(std::get<1>(a))
    {
    }

    vec2f::vec2f(const unit2f& u)
        : x(u.x)
        , y(u.y)
    {
    }

    [[nodiscard]] vec2i
    vec2f::to_i() const
    {
        return {static_cast<int>(x), static_cast<int>(y)};
    }


    float*
    vec2f::get_data_ptr()
    {
        return &x;
    }

    const float*
    vec2f::get_data_ptr() const
    {
        return &x;
    }

    vec2f
    vec2f::get_rotated(const Angle& a) const
    {
        const float nx = x * cos(a) - y * sin(a);
        const float ny = x * sin(a) + y * cos(a);
        return {nx, ny};
    }

    vec2f
    vec2f::get_flipped_y() const
    {
        return {x, -y};
    }

    void
    vec2f::operator+=(const vec2f& rhs)
    {
        x = x + rhs.x;
        y = y + rhs.y;
    }

    void
    vec2f::operator-=(const vec2f& rhs)
    {
        x = x - rhs.x;
        y = y - rhs.y;
    }

    vec2f
    vec2f::operator-() const
    {
        return {-x, -y};
    }

    float
    vec2f::get_length_squared() const
    {
        return x * x + y * y;
    }


    [[nodiscard]] vec2f
    vec2f::from_to(const vec2f& from, const vec2f& to)
    {
        return {to.x - from.x, to.y - from.y};
    }

    void
    vec2f::operator/=(float rhs)
    {
        x = x / rhs;
        y = y / rhs;
    }

    void
    vec2f::operator*=(float rhs)
    {
        x = x * rhs;
        y = y * rhs;
    }

    float
    vec2f::get_length() const
    {
        return sqrt(get_length_squared());
    }

    float
    vec2f::normalize()
    {
        const auto l = get_length();
        *this /= l;
        return l;
    }

    NormalizedAndLength<unit2f, float>
    vec2f::get_normalized_and_length() const
    {
        vec2f r = *this;
        const auto l = r.normalize();
        return {unit2f{r}, l};
    }

    unit2f
    vec2f::get_normalized() const
    {
        return get_normalized_and_length().normalized;
    }

    vec2i::vec2i(const std::tuple<int, int>& a)
        : x(std::get<0>(a))
        , y(std::get<1>(a))
    {
    }

    [[nodiscard]] vec2f
    vec2i::to_f() const
    {
        return {static_cast<float>(x), static_cast<float>(y)};
    }

    int*
    vec2i::get_data_ptr()
    {
        return &x;
    }

    const int*
    vec2i::get_data_ptr() const
    {
        return &x;
    }

    vec2i
    vec2i::get_flipped_y() const
    {
        return {x, -y};
    }

    void
    vec2i::operator+=(const vec2i& rhs)
    {
        x = x + rhs.x;
        y = y + rhs.y;
    }

    void
    vec2i::operator-=(const vec2i& rhs)
    {
        x = x - rhs.x;
        y = y - rhs.y;
    }

    vec2i
    vec2i::operator-() const
    {
        return {-x, -y};
    }


    [[nodiscard]] vec2i
    vec2i::from_to(const vec2i& from, const vec2i& to)
    {
        return {to.x - from.x, to.y - from.y};
    }

    void
    vec2i::operator*=(int rhs)
    {
        x = x * rhs;
        y = y * rhs;
    }
    

    float*
    unit2f::get_data_ptr()
    {
        return &x;
    }

    const float*
    unit2f::get_data_ptr() const
    {
        return &x;
    }

    unit2f
    unit2f::get_rotated(const Angle& a) const
    {
        const float nx = x * cos(a) - y * sin(a);
        const float ny = x * sin(a) + y * cos(a);
        return unit2f{nx, ny};
    }

    unit2f
    unit2f::get_flipped_y() const
    {
        return unit2f{x, -y};
    }

    unit2f
    unit2f::operator-() const
    {
        return unit2f{-x, -y};
    }

    float
    unit2f::get_length_squared() const
    {
        return x * x + y * y;
    }

    vec2f
    unit2f::to_vec() const
    {
        return {x, y};
    }

    [[nodiscard]] bool
    unit2f::is_valid() const
    {
        return is_equal(get_length_squared(), 1.0f);
    }

    unit2f::unit2f(float ax, float ay) : x(ax), y(ay)
    {
        ASSERT(is_valid());
    }
    
    unit2f::unit2f(const vec2f& v) : x(v.x), y(v.y)
    {
        ASSERT(is_valid());
    }


    Scale2f::Scale2f(float ax, float ay)
        : x(ax)
        , y(ay)
    {
    }

    Scale2f::Scale2f(const std::tuple<float, float>& a)
        : x(std::get<0>(a))
        , y(std::get<1>(a))
    {
    }

    float*
    Scale2f::get_data_ptr()
    {
        return &x;
    }

    const float*
    Scale2f::get_data_ptr() const
    {
        return &x;
    }

    Scale2f
    Scale2f::get_rotated(const Angle& a) const
    {
        const float nx = x * cos(a) - y * sin(a);
        const float ny = x * sin(a) + y * cos(a);
        return {nx, ny};
    }

    Scale2f
    Scale2f::get_flipped_y() const
    {
        return {x, -y};
    }

    Scale2f
    Scale2f::operator-() const
    {
        return {-x, -y};
    }
        

    vec2f operator+(const vec2f& lhs, const vec2f& rhs)
    {
        vec2f r = lhs;
        r += rhs;
        return r;
    }

    vec2f operator-(const vec2f& lhs, const vec2f& rhs)
    {
        vec2f r = lhs;
        r -= rhs;
        return r;
    }

    vec2f operator*(const vec2f& lhs, float rhs)
    {
        vec2f r = lhs;
        r *= rhs;
        return r;
    }

    vec2f operator*(float lhs, const vec2f& rhs)
    {
        vec2f r = rhs;
        r *= lhs;
        return r;
    }

    vec2f operator*(const unit2f& lhs, float rhs)
    {
        vec2f r = vec2f{lhs};
        r *= rhs;
        return r;
    }

    vec2f operator*(float lhs, const unit2f& rhs)
    {
        vec2f r{rhs};
        r *= lhs;
        return r;
    }

    vec2f
    operator/(const vec2f& lhs, float rhs)
    {
        vec2f r = lhs;
        r /= rhs;
        return r;
    }

    
    vec2i
    operator+(const vec2i& lhs, const vec2i& rhs)
    {
        vec2i r = lhs;
        r += rhs;
        return r;
    }


    vec2i
    operator-(const vec2i& lhs, const vec2i& rhs)
    {
        vec2i r = lhs;
        r -= rhs;
        return r;
    }


    vec2i operator*(const vec2i& lhs, int rhs)
    {
        vec2i r = lhs;
        r *= rhs;
        return r;
    }


    vec2i operator*(int lhs, const vec2i& rhs)
    {
        vec2i r = rhs;
        r *= lhs;
        return r;
    }


    bool
    operator==(const vec2i& lhs, const vec2i& rhs)
    {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    bool
    operator!=(const vec2i& lhs, const vec2i& rhs)
    {
        return !(lhs == rhs);
    }


    float
    dot(const vec2f& lhs, const vec2f& rhs)
    {
        return lhs.x * rhs.x + lhs.y * rhs.y;
    }
    

    vec2f lerp_vec2f(const vec2f& from, float v, const vec2f& to)
    {
        return
        {
            lerp_float(from.x, v, to.x),
            lerp_float(from.y, v, to.y)
        };
    }

    vec2i lerp_vec2i(const vec2i& from, float v, const vec2i& to)
    {
        return
        {
            static_cast<int>(lerp_float(static_cast<float>(from.x), v, static_cast<float>(to.x))),
            static_cast<int>(lerp_float(static_cast<float>(from.y), v, static_cast<float>(to.y)))
        };
    }

    
    std::string to_string(const vec2f& v)
        { return fmt::format("({}, {})", v.x, v.y); }

    std::string to_string(const unit2f& v)
        { return fmt::format("({}, {})", v.x, v.y); }
    
    std::string to_string(const vec2i& v)
        { return fmt::format("({}, {})", v.x, v.y); }
    

    unit2f create_random_unit(Random* random)
    {
        const auto angle = get_random_angle(random);
        const auto x = cos(angle);
        const auto y = sin(angle);
        return unit2f{x, y};
    }
}
