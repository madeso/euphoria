#ifndef EUPHORIA_CORE_RAYTRACER_H
#define EUPHORIA_CORE_RAYTRACER_H

#include "core/vec3.h"
#include "core/ray.h"
#include "core/range.h"
#include "core/sphere.h"

#include <memory>
#include <vector>
#include <optional>


namespace euphoria::core
{
    struct Image;

    namespace raytracer
    {
        struct HitResult
        {
            HitResult
            (
                float aray_distance,
                const vec3f& aposition,
                const unit3f& anormal
            );

            float ray_distance;
            vec3f position;
            unit3f normal;
        };

        struct Object
        {
            virtual ~Object() = default;

            virtual std::optional<HitResult>
            Hit(const UnitRay3f& ray, const Range<float>& range) const = 0;
        };

        std::shared_ptr<Object>
        CreateSphere(const Sphere& sphere, const vec3f& position);

        struct Scene
        {
            std::vector<std::shared_ptr<Object>> objects;

            std::optional<HitResult>
            Hit(const UnitRay3f& ray, const Range<float>& range) const;
        };

        void
        Raytrace(Image* image, const Scene& scene, int number_of_samples);
    }

}

#endif  // EUPHORIA_CORE_RAYTRACER_H

