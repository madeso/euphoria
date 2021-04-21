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
    Min(const vec3f& lhs, const vec3f& rhs)
    {
#define M(var) Min(lhs.var, rhs.var)
        return vec3f {M(x), M(y), M(z)};
#undef M
    }


    vec3f
    Max(const vec3f& lhs, const vec3f& rhs)
    {
#define M(var) Max(lhs.var, rhs.var)
        return vec3f {M(x), M(y), M(z)};
#undef M
    }


    void
    aabb::extend(const vec3f& vec)
    {
        ASSERT(is_valid());
        min = Min(min, vec);
        max = Max(max, vec);
    }


    void
    aabb::extend(const aabb& aabb)
    {
        ASSERT(is_valid());
        min = Min(min, aabb.min);
        max = Max(max, aabb.max);
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
    aabb::get_random_point(Random* random) const
    {
        const auto x = random->Next(MakeRange(min.x, max.x));
        const auto y = random->Next(MakeRange(min.y, max.y));
        const auto z = random->Next(MakeRange(min.z, max.z));

        return vec3f {x, y, z};
    }
}
