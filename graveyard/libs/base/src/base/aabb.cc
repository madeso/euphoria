#include "base/aabb.h"

#include "assert/assert.h"
#include "base/numeric.h"
#include "base/range.h"
#include "base/random.h"

namespace eu
{
    Aabb::Aabb(const vec3f& amin, const vec3f& amax) : min(amin), max(amax)
    {
        ASSERT(is_valid());
    }


    vec3f
    Aabb::wrap(const vec3f& vec) const
    {
        ASSERT(is_valid());
#define COMP(C) \
    const auto C = ::eu::wrap(make_range(min.C, max.C), vec.C)
        COMP(x);
        COMP(y);
        COMP(z);
#undef COMP
        return vec3f {x, y, z};
    }


    void
    Aabb::extend(const vec3f& vec)
    {
        ASSERT(is_valid());
        min = eu::min(min, vec);
        max = eu::max(max, vec);
    }


    void
    Aabb::extend(const Aabb& aabb)
    {
        ASSERT(is_valid());
        min = eu::min(min, aabb.min);
        max = eu::max(max, aabb.max);
    }


    Aabb
    Aabb::create_empty()
    {
        return Aabb {zero3f, zero3f};
    }


    vec3f
    Aabb::get_size() const
    {
        ASSERT(is_valid());
        return vec3f::from_to(min, max);
    }


    bool
    Aabb::is_valid() const
    {
#define M(var) min.var <= max.var
        return M(x) && M(y) && M(z);
#undef M
    }


    vec3f
    Aabb::get_offset() const
    {
        ASSERT(is_valid());
        return min;
    }


    void
    Aabb::offset(const vec3f& vec)
    {
        min += vec;
        max += vec;
    }


    Aabb
    Aabb::offset_copy(const vec3f& vec) const
    {
        auto self = *this;
        self.offset(vec);
        return self;
    }


    [[nodiscard]] std::vector<vec3f>
    Aabb::calc_all_corners() const
    {
        return
        {
            {min.x, min.y, min.z}, {max.x, min.y, min.z},
            {min.x, min.y, max.z}, {max.x, min.y, max.z},
            {min.x, max.y, min.z}, {max.x, max.y, min.z},
            {min.x, max.y, max.z}, {max.x, max.y, max.z},
        };
    }

    std::string
    to_string(const Aabb& a)
    {
        return fmt::format("{{ {}, {} }}", a.min, a.max);
    }

    vec3f
    get_random_point(Random* rand, const Aabb& a)
    {
        const auto x = get_random_in_range(rand, make_range(a.min.x, a.max.x));
        const auto y = get_random_in_range(rand, make_range(a.min.y, a.max.y));
        const auto z = get_random_in_range(rand, make_range(a.min.z, a.max.z));

        return vec3f {x, y, z};
    }
}
