#pragma once



#include "core/vec3.h"


namespace euphoria::core
{
    struct Random;

    struct Aabb
    {
        Aabb(const vec3f& amin, const vec3f& amax);


        [[nodiscard]] vec3f
        wrap(const vec3f& vec) const;


        void
        extend(const vec3f& vec);


        void
        extend(const Aabb& aabb);


        [[nodiscard]] static Aabb
        create_empty();


        [[nodiscard]] vec3f
        get_size() const;


        [[nodiscard]] bool
        is_valid() const;


        [[nodiscard]] vec3f
        get_offset() const;


        void
        offset(const vec3f& vec);


        [[nodiscard]] Aabb
        offset_copy(const vec3f& vec) const;


        [[nodiscard]] std::vector<vec3f>
        calculate_all_corners() const;


        vec3f min;
        vec3f max;
    };

    template <typename Stream>
    Stream&
    operator<<(Stream& s, const Aabb& a)
    {
        s << "{" << a.min << ", " << a.max << "}";
        return s;
    }

    vec3f
    get_random_point(Random* rand, const Aabb&);

}
