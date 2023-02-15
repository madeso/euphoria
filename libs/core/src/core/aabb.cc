#include "core/aabb.h"

#include "assert/assert.h"
#include "core/numeric.h"
#include "core/range.h"
#include "core/random.h"

namespace euphoria::core
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
    const auto C = ::euphoria::core::wrap(make_range(min.C, max.C), vec.C)
        COMP(x);
        COMP(y);
        COMP(z);
#undef COMP
        return vec3f {x, y, z};
    }


    vec3f
    min(const vec3f& lhs, const vec3f& rhs)
    {
#define M(var) core::min(lhs.var, rhs.var)
        return vec3f {M(x), M(y), M(z)};
#undef M
    }


    vec3f
    max(const vec3f& lhs, const vec3f& rhs)
    {
#define M(var) core::max(lhs.var, rhs.var)
        return vec3f {M(x), M(y), M(z)};
#undef M
    }


    void
    Aabb::extend(const vec3f& vec)
    {
        ASSERT(is_valid());
        min = core::min(min, vec);
        max = core::max(max, vec);
    }


    void
    Aabb::extend(const Aabb& aabb)
    {
        ASSERT(is_valid());
        min = core::min(min, aabb.min);
        max = core::max(max, aabb.max);
    }


    Aabb
    Aabb::create_empty()
    {
        return Aabb {vec3f::zero(), vec3f::zero()};
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
    Aabb::calculate_all_corners() const
    {
        return
        {
            {min.x, min.y, min.z}, {max.x, min.y, min.z},
            {min.x, min.y, max.z}, {max.x, min.y, max.z},
            {min.x, max.y, min.z}, {max.x, max.y, min.z},
            {min.x, max.y, max.z}, {max.x, max.y, max.z},
        };
    }

    std::ostream&
    operator<<(std::ostream& s, const Aabb& a)
    {
        s << "{" << a.min << ", " << a.max << "}";
        return s;
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
