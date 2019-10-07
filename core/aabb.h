#ifndef EUPHORIA_AABB_H
#define EUPHORIA_AABB_H

#include "core/vec3.h"

namespace euphoria::core
{
    class Random;

    class Aabb
    {
    public:
        Aabb(const vec3f& amin, const vec3f& amax);

        vec3f
        Wrap(const vec3f& vec) const;

        void
        Extend(const vec3f& vec);

        void
        Extend(const Aabb& aabb);

        static Aabb
        Empty();

        vec3f
        GetSize() const;

        bool
        IsValid() const;

        vec3f
        GetOffset() const;

        const vec3f&
        GetMin() const;

        const vec3f&
        GetMax() const;

        vec3f
        RandomPoint(Random* random);

        vec3f min;
        vec3f max;
    };

    template <typename Stream>
    Stream&
    operator<<(Stream& s, const Aabb& a)
    {
        s << "{" << a.GetMin() << ", " << a.GetMax() << "}";
        return s;
    }

}  // namespace euphoria::core

#endif  // EUPHORIA_AABB_H
