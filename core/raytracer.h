#ifndef EUPHORIA_CORE_RAYTRACER_H
#define EUPHORIA_CORE_RAYTRACER_H

#include "core/vec3.h"
#include "core/ray.h"
#include "core/range.h"
#include "core/sphere.h"

#include <memory>
#include <vector>


namespace euphoria::core
{
    struct Image;

    namespace raytracer
    {
        struct HitResult
        {
            HitResult
            (
                bool acollided,
                float aray_distance,
                const vec3f& aposition,
                const unit3f& anormal
            );

            bool collided;
            float ray_distance;
            vec3f position;
            unit3f normal;
        };

        struct Object
        {
            virtual ~Object() = default;

            virtual HitResult
            Hit(const UnitRay3f& ray, const Range<float>& range) const = 0;
        };

        std::shared_ptr<Object>
        CreateSphere(const Sphere& sphere, const vec3f& position);

        struct Scene
        {
            std::vector<std::shared_ptr<Object>> objects;

            HitResult
            Hit(const UnitRay3f& ray, const Range<float>& range) const;
        };

        void
        Raytrace(Image* image, const Scene& scene);
    }

}

#endif  // EUPHORIA_CORE_RAYTRACER_H

