#include "core/aabb.h"

#include "core/assert.h"
#include "core/numeric.h"
#include "core/range.h"
#include "core/random.h"

namespace euphoria::core
{
    Aabb::Aabb(const vec3f& amin, const vec3f& amax) : min(amin), max(amax)
    {
        ASSERT(IsValid());
    }

    const vec3f&
    Aabb::GetMin() const
    {
        ASSERT(IsValid());
        return min;
    }

    const vec3f&
    Aabb::GetMax() const
    {
        ASSERT(IsValid());
        return max;
    }

    vec3f
    Aabb::Wrap(const vec3f& vec) const
    {
        ASSERT(IsValid());
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
    Aabb::Extend(const vec3f& vec)
    {
        ASSERT(IsValid());
        min = Min(min, vec);
        max = Max(max, vec);
    }

    void
    Aabb::Extend(const Aabb& aabb)
    {
        ASSERT(IsValid());
        min = Min(min, aabb.min);
        max = Max(max, aabb.max);
    }

    Aabb
    Aabb::Empty()
    {
        return Aabb {vec3f::Zero(), vec3f::Zero()};
    }

    vec3f
    Aabb::GetSize() const
    {
        ASSERT(IsValid());
        return vec3f::FromTo(min, max);
    }

    bool
    Aabb::IsValid() const
    {
#define M(var) min.var <= max.var
        return M(x) && M(y) && M(z);
#undef M
    }

    vec3f
    Aabb::GetOffset() const
    {
        ASSERT(IsValid());
        return vec3f::FromTo(vec3f::Zero(), min);
    }

    vec3f
    Aabb::RandomPoint(Random* random)
    {
        const auto x = random->Next(MakeRange(GetMin().x, GetMax().x));
        const auto y = random->Next(MakeRange(GetMin().y, GetMax().y));
        const auto z = random->Next(MakeRange(GetMin().z, GetMax().z));

        return vec3f {x, y, z};
    }

}  // namespace euphoria::core
