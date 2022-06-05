#pragma once



#include "core/vec3.h"


namespace euphoria::core
{
    struct Random;

    struct Aabb
    {
        Aabb(const Vec3f& amin, const Vec3f& amax);


        [[nodiscard]] Vec3f
        wrap(const Vec3f& vec) const;


        void
        extend(const Vec3f& vec);


        void
        extend(const Aabb& aabb);


        [[nodiscard]] static Aabb
        create_empty();


        [[nodiscard]] Vec3f
        get_size() const;


        [[nodiscard]] bool
        is_valid() const;


        [[nodiscard]] Vec3f
        get_offset() const;


        void
        offset(const Vec3f& vec);


        [[nodiscard]] Aabb
        offset_copy(const Vec3f& vec) const;


        [[nodiscard]] std::vector<Vec3f>
        calculate_all_corners() const;


        Vec3f min;
        Vec3f max;
    };

    template <typename Stream>
    Stream&
    operator<<(Stream& s, const Aabb& a)
    {
        s << "{" << a.min << ", " << a.max << "}";
        return s;
    }

    Vec3f
    get_random_point(Random* rand, const Aabb&);

}
