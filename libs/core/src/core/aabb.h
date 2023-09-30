#pragma once



#include "base/vec3.h"

namespace eu
{
    struct Random;
}

namespace eu::core
{
    struct Aabb
    {
        vec3f min;
        vec3f max;

        Aabb(const vec3f& amin, const vec3f& amax);
        [[nodiscard]] static Aabb create_empty();

        void extend(const vec3f& vec);
        void extend(const Aabb& aabb);
        void offset(const vec3f& vec);

        [[nodiscard]] vec3f wrap(const vec3f& vec) const;
        [[nodiscard]] Aabb offset_copy(const vec3f& vec) const;

        [[nodiscard]] vec3f get_size() const;
        [[nodiscard]] bool is_valid() const;
        [[nodiscard]] vec3f get_offset() const;


        [[nodiscard]] std::vector<vec3f> calc_all_corners() const;
    };

    std::string to_string(const Aabb& a);
    vec3f get_random_point(Random* rand, const Aabb&);
}

ADD_DEFAULT_FORMATTER(eu::core::Aabb, std::string, eu::core::to_string);
