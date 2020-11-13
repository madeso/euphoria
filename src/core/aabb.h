#ifndef EUPHORIA_AABB_H
#define EUPHORIA_AABB_H

#include "core/vec3.h"

namespace euphoria::core
{
    struct Random;

    struct Aabb
    {
        Aabb(const vec3f& amin, const vec3f& amax);


        vec3f
        Wrap(const vec3f& vec) const;


        void
        Extend(const vec3f& vec);


        void
        Extend(const Aabb& aabb);


        [[nodiscard]] static Aabb
        Empty();


        vec3f
        GetSize() const;


        bool
        IsValid() const;


        vec3f
        GetOffset() const;


        void
        Offset(const vec3f& vec);


        Aabb
        OffsetCopy(const vec3f& vec) const;


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

}

#endif  // EUPHORIA_AABB_H
