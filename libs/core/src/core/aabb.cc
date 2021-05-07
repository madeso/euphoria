#include "core/aabb.h"

#include "core/assert.h"
#include "core/numeric.h"
#include "core/range.h"
#include "core/random.h"

namespace euphoria::core
{
    aabb::aabb(const vec3f& amin, const vec3f& amax) : min(amin), max(amax)
    {
        ASSERT(is_valid());
    }


    vec3f
    aabb::wrap(const vec3f& vec) const
    {
        ASSERT(is_valid());
#define COMP(C)                                                                \
    const auto C = ::euphoria::core::Wrap(MakeRange(min.C, max.C), vec.C)
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
    aabb::extend(const vec3f& vec)
    {
        ASSERT(is_valid());
        min = core::min(min, vec);
        max = core::max(max, vec);
    }


    void
    aabb::extend(const aabb& aabb)
    {
        ASSERT(is_valid());
        min = core::min(min, aabb.min);
        max = core::max(max, aabb.max);
    }


    aabb
    aabb::Empty()
    {
        return aabb {vec3f::zero(), vec3f::zero()};
    }


    vec3f
    aabb::get_size() const
    {
        ASSERT(is_valid());
        return vec3f::from_to(min, max);
    }


    bool
    aabb::is_valid() const
    {
#define M(var) min.var <= max.var
        return M(x) && M(y) && M(z);
#undef M
    }


    vec3f
    aabb::get_offset() const
    {
        ASSERT(is_valid());
        return min;
    }


    void
    aabb::offset(const vec3f& vec)
    {
        min += vec;
        max += vec;
    }


    aabb
    aabb::offset_copy(const vec3f& vec) const
    {
        auto self = *this;
        self.offset(vec);
        return self;
    }


    vec3f
    aabb::get_random_point(random* rand) const
    {
        const auto x = get_next_in_range(rand, MakeRange(min.x, max.x));
        const auto y = get_next_in_range(rand, MakeRange(min.y, max.y));
        const auto z = get_next_in_range(rand, MakeRange(min.z, max.z));

        return vec3f {x, y, z};
    }
}
