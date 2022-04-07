#include "core/aabb.h"

#include "assert/assert.h"
#include "core/numeric.h"
#include "core/range.h"
#include "core/random.h"

namespace euphoria::core
{
    Aabb::Aabb(const Vec3f& amin, const Vec3f& amax) : min(amin), max(amax)
    {
        ASSERT(is_valid());
    }


    Vec3f
    Aabb::wrap(const Vec3f& vec) const
    {
        ASSERT(is_valid());
#define COMP(C) \
    const auto C = ::euphoria::core::wrap(make_range(min.C, max.C), vec.C)
        COMP(x);
        COMP(y);
        COMP(z);
#undef COMP
        return Vec3f {x, y, z};
    }


    Vec3f
    min(const Vec3f& lhs, const Vec3f& rhs)
    {
#define M(var) core::min(lhs.var, rhs.var)
        return Vec3f {M(x), M(y), M(z)};
#undef M
    }


    Vec3f
    max(const Vec3f& lhs, const Vec3f& rhs)
    {
#define M(var) core::max(lhs.var, rhs.var)
        return Vec3f {M(x), M(y), M(z)};
#undef M
    }


    void
    Aabb::extend(const Vec3f& vec)
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
        return Aabb {Vec3f::zero(), Vec3f::zero()};
    }


    Vec3f
    Aabb::get_size() const
    {
        ASSERT(is_valid());
        return Vec3f::from_to(min, max);
    }


    bool
    Aabb::is_valid() const
    {
#define M(var) min.var <= max.var
        return M(x) && M(y) && M(z);
#undef M
    }


    Vec3f
    Aabb::get_offset() const
    {
        ASSERT(is_valid());
        return min;
    }


    void
    Aabb::offset(const Vec3f& vec)
    {
        min += vec;
        max += vec;
    }


    Aabb
    Aabb::offset_copy(const Vec3f& vec) const
    {
        auto self = *this;
        self.offset(vec);
        return self;
    }


    [[nodiscard]] std::vector<Vec3f>
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


    Vec3f
    get_random_point(Random* rand, const Aabb& a)
    {
        const auto x = get_random_in_range(rand, make_range(a.min.x, a.max.x));
        const auto y = get_random_in_range(rand, make_range(a.min.y, a.max.y));
        const auto z = get_random_in_range(rand, make_range(a.min.z, a.max.z));

        return Vec3f {x, y, z};
    }
}
