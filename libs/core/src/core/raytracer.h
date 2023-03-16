#pragma once

#include "core/vec3.h"
#include "core/ray.h"
#include "core/range.h"
#include "core/sphere.h"
#include "core/rgb.h"

#include <memory>

#include <optional>


namespace euphoria::core
{
    struct Image;

    namespace raytracer
    {
        struct HitResult;


        struct ScatterResult
        {
            rgb attenuation;
            UnitRay3f scattered;
        };


        struct Material
        {
            Material() = default;
            virtual ~Material() = default;

            Material(const Material& other) = delete;
            void operator=(const Material&) = delete;
            Material(Material&& other) = delete;
            void operator=(Material&&) = delete;

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
            float ray_distance;
            vec3f position;
            unit3f normal;
            std::shared_ptr<raytracer::Material> material;

            HitResult
            (
                float aray_distance,
                const vec3f& aposition,
                const unit3f& anormal,
                std::shared_ptr<raytracer::Material> amaterial
            );
        };


        struct Object
        {
            Object() = default;
            virtual ~Object() = default;

            Object(const Object&) = delete;
            Object(Object&&) = delete;
            void operator=(const Object&) = delete;
            void operator=(Object&&) = delete;

            [[nodiscard]] virtual std::optional<HitResult>
            hit(const UnitRay3f& ray, const Range<float>& range) const = 0;
        };


        std::shared_ptr<Object> create_sphere
        (
            const Sphere& sphere,
            const vec3f& position,
            std::shared_ptr<Material> material
        );


        std::shared_ptr<Material> create_diffuse_material
        (
            const rgb& albedo
        );


        std::shared_ptr<Material> create_metal_material
        (
            const rgb& albedo,
            // 0-1, 0=clear
            float fuzz
        );


        std::shared_ptr<Material> create_dielectric_material
        (
            const rgb& albedo,
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

