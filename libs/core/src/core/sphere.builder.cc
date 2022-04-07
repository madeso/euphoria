#include "core/sphere.builder.h"

#include "assert/assert.h"

#include "core/cint.h"
#include "core/numeric.h"


namespace euphoria::core
{
    Vec3f
    calculate_center(const std::vector<Vec3f>& positions)
    {
        ASSERT(positions.empty() == false);

        auto center = Vec3f::zero();

        for(const auto& p: positions)
        {
            center += p;
        }

        return center / core::c_sizet_to_float(positions.size());
    }
    
    SphereAndPosition
    build_bounding_sphere(const std::vector<Vec3f>& positions)
    {
        ASSERT(positions.empty() == false);

        const auto center = calculate_center(positions);
        float radius_squared = 0;

        for(const auto& p: positions)
        {
            const auto lensq = Vec3f::from_to(p, center).get_length_squared();
            if(lensq > radius_squared)
            {
                radius_squared = lensq;
            }
        }

        return
        {
            Sphere{core::sqrt(radius_squared)},
            center
        };
    }
}
