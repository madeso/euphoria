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
    struct image;

    namespace raytracer
    {
        struct hit_result;


        struct scatter_result
        {
            rgb attenuation;
            unit_ray3f scattered;
        };


        struct material
        {
            material() = default;
            virtual ~material() = default;

            NONCOPYABLE(material);

            [[nodiscard]] virtual std::optional<scatter_result>
            scatter
            (
                const unit_ray3f& ray,
                const hit_result& hit,
                random* random
            ) = 0;
        };


        struct hit_result
        {
            hit_result
            (
                float aray_distance,
                const vec3f& aposition,
                const unit3f& anormal,
                std::shared_ptr<raytracer::material> amaterial
            );

            float ray_distance;
            vec3f position;
            unit3f normal;
            std::shared_ptr<raytracer::material> material;
        };


        struct object
        {
            object() = default;
            virtual ~object() = default;

            NONCOPYABLE(object);

            [[nodiscard]] virtual std::optional<hit_result>
            hit(const unit_ray3f& ray, const range<float>& range) const = 0;
        };


        std::shared_ptr<object>
        create_sphere
        (
            const sphere& sphere,
            const vec3f& position,
            std::shared_ptr<material> material
        );


        std::shared_ptr<material>
        create_diffuse_material
        (
            const rgb& albedo
        );


        std::shared_ptr<material>
        create_metal_material
        (
            const rgb& albedo,
            // 0-1, 0=clear
            float fuzz
        );


        std::shared_ptr<material>
        create_dielectric_material
        (
            const rgb& albedo,
            float refractive_index
        );


        struct scene
        {
            std::vector<std::shared_ptr<object>> objects;

            [[nodiscard]] std::optional<hit_result>
            hit(const unit_ray3f& ray, const range<float>& range) const;
        };


        void
        raytrace(image* image, const raytracer::scene& scene, int number_of_samples);
    }

}

