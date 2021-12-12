#pragma once

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
            scatter
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
                const Vec3f& aposition,
                const Unit3f& anormal,
                std::shared_ptr<raytracer::Material> amaterial
            );

            float ray_distance;
            Vec3f position;
            Unit3f normal;
            std::shared_ptr<raytracer::Material> material;
        };


        struct Object
        {
            Object() = default;
            virtual ~Object() = default;

            NONCOPYABLE(Object);

            [[nodiscard]] virtual std::optional<HitResult>
            hit(const UnitRay3f& ray, const Range<float>& range) const = 0;
        };


        std::shared_ptr<Object>
        create_sphere
        (
            const Sphere& sphere,
            const Vec3f& position,
            std::shared_ptr<Material> material
        );


        std::shared_ptr<Material>
        create_diffuse_material
        (
            const Rgb& albedo
        );


        std::shared_ptr<Material>
        create_metal_material
        (
            const Rgb& albedo,
            // 0-1, 0=clear
            float fuzz
        );


        std::shared_ptr<Material>
        create_dielectric_material
        (
            const Rgb& albedo,
            float refractive_index
        );


        struct Scene
        {
            std::vector<std::shared_ptr<Object>> objects;

            [[nodiscard]] std::optional<HitResult>
            hit(const UnitRay3f& ray, const Range<float>& range) const;
        };


        void
        raytrace(Image* image, const raytracer::Scene& scene, int number_of_samples);
    }

}

