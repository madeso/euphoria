#include "base/vec4.h"

namespace eu
{
    v4::v4(float ax, float ay, float az, float aw)
        : x(ax), y(ay), z(az), w(aw)
    {
    }

    v4::v4(const v3 &a, float aw)
        : x(a.x), y(a.y), z(a.z), w(aw)
    {
    }

    v4::v4(const float *a)
        : x(a[0]), y(a[1]), z(a[2]), w(a[3])
    {
    }

    v3
    v4::to_vec3(float ww) const
    {
        ASSERTX(is_equal(w, ww), w, ww);
        return {x, y, z};
    }

    float *
    v4::get_data_ptr()
    {
        return &x;
    }

    const float *
    v4::get_data_ptr() const
    {
        return &x;
    }

    std::string string_from(const v4 &v)
    {
        return fmt::format("({}, {}, {}, {})", v.x, v.y, v.z, v.w);
    }
}
