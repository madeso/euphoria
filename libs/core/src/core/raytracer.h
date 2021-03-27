#ifndef EUPHORIA_CORE_RAYTRACER_H
#define EUPHORIA_CORE_RAYTRACER_H

#include "core/vec3.h"
#include "core/ray.h"
#include "core/range.h"
#include "core/sphere.h"
#include "core/rgb.h"
#include "core/noncopyable.h"

#include <memory>
#include <vector>
#include <optional>


namespace euphoria::core
{
    struct Image;

    namespace raytracer
    {
        struct HitResult;


        struct ScatterResult
        {
            Rgb attenuation;
            UnitRay3f scattered;
        };


        struct Material
        {
            Material() = default;
            virtual ~Material() = default;

            NONCOPYABLE(Material);

            [[nodiscard]] virtual std::optional<ScatterResult>
            Scatter
            (
                const UnitRay3f& ray,
                const HitResult& hit,
                Random* random
            ) = 0;
        };


        struct HitResult
        {
            HitResult
            (
                float aray_distance,
                const vec3f& aposition,
                const unit3f& anormal,
                std::shared_ptr<Material> amaterial
            );

            float ray_distance;
            vec3f position;
            unit3f normal;
            std::shared_ptr<Material> material;
        };


        struct Object
        {
            Object() = default;
            virtual ~Object() = default;

            NONCOPYABLE(Object);

            [[nodiscard]] virtual std::optional<HitResult>
            Hit(const UnitRay3f& ray, const Range<float>& range) const = 0;
        };


        std::shared_ptr<Object>
        CreateSphere
        (
            const Sphere& sphere,
            const vec3f& position,
            std::shared_ptr<Material> material
        );


        std::shared_ptr<Material>
        CreateDiffuseMaterial
        (
            const Rgb& albedo
        );


        std::shared_ptr<Material>
        CreateMetalMaterial
        (
            const Rgb& albedo,
            // 0-1, 0=clear
            float fuzz
        );


        std::shared_ptr<Material>
        CreateDielectricMaterial
        (
            const Rgb& albedo,
            float refractive_index
        );


        struct Scene
        {
            std::vector<std::shared_ptr<Object>> objects;

            [[nodiscard]] std::optional<HitResult>
            Hit(const UnitRay3f& ray, const Range<float>& range) const;
        };


        void
        Raytrace(Image* image, const Scene& scene, int number_of_samples);
    }

}

#endif  // EUPHORIA_CORE_RAYTRACER_H

