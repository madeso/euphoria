#ifndef EUPHORIA_AABB_H
#define EUPHORIA_AABB_H

#include "core/vec3.h"

namespace euphoria::core
{
    struct Random;

    struct aabb
    {
        aabb(const vec3f& amin, const vec3f& amax);


        [[nodiscard]] vec3f
        wrap(const vec3f& vec) const;


        void
        extend(const vec3f& vec);


        void
        extend(const aabb& aabb);


        [[nodiscard]] static aabb
        Empty();


        [[nodiscard]] vec3f
        get_size() const;


        [[nodiscard]] bool
        is_valid() const;


        [[nodiscard]] vec3f
        get_offset() const;


        void
        offset(const vec3f& vec);


        [[nodiscard]] aabb
        offset_copy(const vec3f& vec) const;


        [[nodiscard]] vec3f
        get_random_point(Random* random) const;


        vec3f min;
        vec3f max;
    };

    template <typename Stream>
    Stream&
    operator<<(Stream& s, const aabb& a)
    {
        s << "{" << a.min << ", " << a.max << "}";
        return s;
    }

}

#endif  // EUPHORIA_AABB_H
