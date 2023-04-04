#include "core/vec4.h"


namespace euphoria::core
{
    vec4f::vec4f(float a)
        : x(a)
        , y(a)
        , z(a)
        , w(a)
    {
    }


    vec4f::vec4f(float ax, float ay, float az, float aw)
        : x(ax)
        , y(ay)
        , z(az)
        , w(aw)
    {
    }


    vec4f::vec4f(const vec3f& a, float aw)
        : x(a.x)
        , y(a.y)
        , z(a.z)
        , w(aw)
    {
    }


    vec4f::vec4f(const Scale3f& a)
        : x(a.x)
        , y(a.y)
        , z(a.z)
        , w(1)
    {
    }


    vec4f::vec4f(const float* a)
        : x(a[0])
        , y(a[1])
        , z(a[2])
        , w(a[3])
    {
    }


    vec3f
    vec4f::to_vec3(float ww) const
    {
        ASSERTX(is_equal(w, ww), w, ww);
        return {x, y, z};
    }


    vec3f
    vec4f::to_vec3() const
    {
        return {x, y, z};
    }


    float*
    vec4f::get_data_ptr()
    {
        return &x;
    }


    const float*
    vec4f::get_data_ptr() const
    {
        return &x;
    }


    std::string to_string(const vec4f& v)
    {
        return fmt::format("({}, {}, {}, {})", v.x, v.y, v.z, v.w);
    }
}
