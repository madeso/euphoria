#include "core/sphere.builder.h"

#include "assert/assert.h"

#include "base/cint.h"
#include "base/numeric.h"


namespace eu::core
{
    vec3f
    calc_center(const std::vector<vec3f>& positions)
    {
        ASSERT(positions.empty() == false);

        auto center = zero3f;

        for(const auto& p: positions)
        {
            center += p;
        }

        return center / c_sizet_to_float(positions.size());
    }
    
    SphereAndPosition
    build_bounding_sphere(const std::vector<vec3f>& positions)
    {
        ASSERT(positions.empty() == false);

        const auto center = calc_center(positions);
        float radius_squared = 0;

        for(const auto& p: positions)
        {
            const auto lensq = vec3f::from_to(p, center).get_length_squared();
            if(lensq > radius_squared)
            {
                radius_squared = lensq;
            }
        }

        return
        {
            Sphere{sqrt(radius_squared)},
            center
        };
    }
}
