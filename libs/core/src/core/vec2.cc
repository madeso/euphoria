#include "core/vec2.h"

#include "core/random.h"
#include "core/angle.h"



namespace euphoria::core
{
    vec2f::vec2f(float ax, float ay)
        : x(ax)
        , y(ay)
    {
    }

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
    vec2f::toi() const
    {
        return vec2i{static_cast<int>(x), static_cast<int>(y)};
    }

    [[nodiscard]] vec2f
    vec2f::zero()
    {
        return {0, 0};
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
    vec2f::get_rotated(const angle& a) const
    {
        const float nx = x * cos(a) - y * sin(a);
        const float ny = x * sin(a) + y * cos(a);
        return vec2f(nx, ny);
    }

    vec2f
    vec2f::get_flipped_y() const
    {
        return vec2f(x, -y);
    }

    float
    vec2f::get_component_sum() const
    {
        return x + y;
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
        return vec2f(-x, -y);
    }

    float
    vec2f::get_length_squared() const
    {
        return x * x + y * y;
    }


    [[nodiscard]] vec2f
    vec2f::from_to(const vec2f& from, const vec2f& to)
    {
        return vec2f {to.x - from.x, to.y - from.y};
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



    vec2i::vec2i(int ax, int ay)
        : x(ax)
        , y(ay)
    {
    }

    vec2i::vec2i(const std::tuple<int, int>& a)
        : x(std::get<0>(a))
        , y(std::get<1>(a))
    {
    }

    [[nodiscard]] vec2f
    vec2i::tof() const
    {
        return vec2f{static_cast<float>(x), static_cast<float>(y)};
    }

    [[nodiscard]] vec2i
    vec2i::zero()
    {
        return {0, 0};
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
        return vec2i(x, -y);
    }

    int
    vec2i::get_component_sum() const
    {
        return x + y;
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
        return vec2i(-x, -y);
    }


    [[nodiscard]] vec2i
    vec2i::from_to(const vec2i& from, const vec2i& to)
    {
        return vec2i {to.x - from.x, to.y - from.y};
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
    unit2f::get_rotated(const angle& a) const
    {
        const float nx = x * cos(a) - y * sin(a);
        const float ny = x * sin(a) + y * cos(a);
        return unit2f(nx, ny);
    }

    unit2f
    unit2f::get_flipped_y() const
    {
        return unit2f(x, -y);
    }

    float
    unit2f::get_component_sum() const
    {
        return x + y;
    }

    unit2f
    unit2f::operator-() const
    {
        return unit2f(-x, -y);
    }

    float
    unit2f::get_length_squared() const
    {
        return x * x + y * y;
    }

    vec2f
    unit2f::to_vec() const
    {
        return vec2f(x, y);
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
    Scale2f::get_rotated(const angle& a) const
    {
        const float nx = x * cos(a) - y * sin(a);
        const float ny = x * sin(a) + y * cos(a);
        return Scale2f(nx, ny);
    }

    Scale2f
    Scale2f::get_flipped_y() const
    {
        return Scale2f(x, -y);
    }
    
    float
    Scale2f::get_component_sum() const
    {
        return x + y;
    }

    Scale2f
    Scale2f::operator-() const
    {
        return Scale2f(-x, -y);
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

    vec2f
    component_multiply(const vec2f& lhs, const vec2f& rhs)
    {
        return vec2f(lhs.x * rhs.x, lhs.y * rhs.y);
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


    vec2i
    component_multiply(const vec2i& lhs, const vec2i& rhs)
    {
        return vec2i(lhs.x * rhs.x, lhs.y * rhs.y);
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
    

    vec2f
    Vec2fTransform::transform(const vec2f& from, float v, const vec2f& to)
    {
        return vec2f
        (
            FloatTransform::transform(from.x, v, to.x),
            FloatTransform::transform(from.y, v, to.y)
        );
    }

    vec2i
    Vec2iTransform::transform(const vec2i& from, float v, const vec2i to)
    {
        return vec2i
        (
            static_cast<int>(FloatTransform::transform(static_cast<float>(from.x), v, static_cast<float>(to.x))),
            static_cast<int>(FloatTransform::transform(static_cast<float>(from.y), v, static_cast<float>(to.y)))
        );
    }

    
    std::ostream&
    operator<<(std::ostream& s, const vec2f& v)
    {
        s << "(" << v.x << ", " << v.y << ")";
        return s;
    }

    std::ostream&
    operator<<(std::ostream& s, const unit2f& v)
    {
        s << "(" << v.x << ", " << v.y << ")";
        return s;
    }

    
    std::ostream&
    operator<<(std::ostream& s, const vec2i& v)
    {
        s << "(" << v.x << ", " << v.y << ")";
        return s;
    }
    

    unit2f create_random_unit(Random* random)
    {
        const auto angle = angle::random(random);
        const auto x = cos(angle);
        const auto y = sin(angle);
        return unit2f{x, y};
    }
}
