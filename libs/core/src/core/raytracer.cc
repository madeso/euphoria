#include "core/raytracer.h"

#include "core/image.h"

#include "core/intersection.h"
#include "core/sphere.h"
#include "core/random.h"
#include "core/polarcoord.h"
#include "core/numeric.h"
#include "core/angle.h"

#include <limits>
#include <cmath>

namespace euphoria::core::raytracer
{
    hit_result::hit_result
    (
        float aray_distance,
        const vec3f& aposition,
        const unit3f& anormal,
        std::shared_ptr<raytracer::material> amaterial
    )
        : ray_distance(aray_distance)
        , position(aposition)
        , normal(anormal)
        , material(amaterial)
    {
    }


    struct sphere_object : object
    {
        core::sphere sphere;
        vec3f position;
        std::shared_ptr<material> material;

        sphere_object
        (
            const core::sphere& asphere,
            const vec3f& aposition,
            std::shared_ptr<raytracer::material> amaterial
        )
            : sphere(asphere)
            , position(aposition)
            , material(amaterial)
        {
        }

        [[nodiscard]] std::optional<hit_result>
        hit(const unit_ray3f& ray, const range<float>& range) const override
        {
            const auto hit_index = get_intersection
            (
                ray,
                sphere,
                position
            );
            if(is_within(range, hit_index))
            {
                const auto hit_position = ray.get_point(hit_index);
                const auto hit_normal = vec3f::from_to(position, hit_position).get_normalized();
                return hit_result
                {
                    hit_index,
                    hit_position,
                    hit_normal,
                    material
                };
            }
            else
            {
                return std::nullopt;
            }
        }
    };


    std::shared_ptr<object>
    create_sphere
    (
        const sphere& sphere,
        const vec3f& position,
        std::shared_ptr<raytracer::material> material
    )
    {
        return std::make_shared<sphere_object>
        (
            sphere,
            position,
            material
        );
    }


    std::optional<hit_result>
    scene::hit(const unit_ray3f& ray, const range<float>& range) const
    {
        std::optional<hit_result> r = std::nullopt;

        for(const auto& o: objects)
        {
            const auto h = o->hit(ray, range);
            if(r.has_value() == false)
            {
                r = h;
            }
            else
            {
                if(h.has_value())
                {
                    if(h->ray_distance < r->ray_distance)
                    {
                        r = h;
                    }
                }
            }
        }

        return r;
    }


    rgb
    crgb(const unit3f& normal)
    {
        return rgb
        {
            (normal.x + 1)/2,
            (normal.y + 1)/2,
            (normal.z + 1)/2
        };
    }


    vec3f get_random_vec3_in_unit_sphere(random* random)
    {
        return get_random_unit3(random) * random->get_next_float01();
    }


    struct diffuse_material : public material
    {
        rgb albedo;

        explicit diffuse_material(const rgb& aalbedo)
            : albedo(aalbedo)
        {
        }

        std::optional<scatter_result>
        scatter
        (
            const unit_ray3f& /*ray*/,
            const hit_result& hit,
            random* random
        ) override
        {
            const auto target = hit.position + hit.normal + get_random_vec3_in_unit_sphere(random);
            const auto reflected_ray = unit_ray3f::from_to(hit.position, target);

            return scatter_result
            {
                albedo,
                reflected_ray
            };
        }
    };


    vec3f reflect(const vec3f& v, const unit3f& normal) {
        return v - 2*dot(v,normal)*normal;
    }


    struct metal_material : material
    {
        rgb albedo;
        float fuzz;

        explicit metal_material
        (
            const rgb& aalbedo,
            float afuzz
        )
            : albedo(aalbedo)
            , fuzz(keep_within(r01, afuzz))
        {
        }

        std::optional<scatter_result>
        scatter
        (
            const unit_ray3f& ray,
            const hit_result& hit,
            random* random
        ) override
        {
            const auto reflected = reflect
            (
                ray.dir,
                hit.normal
            );
            const auto scattered = unit_ray3f::from_to
            (
                hit.position,
                reflected + fuzz * get_random_vec3_in_unit_sphere(random)
            );
            const auto scatter_dot = dot
            (
                scattered.dir,
                hit.normal
            );
            if(scatter_dot > 0)
            {
                return scatter_result
                {
                    albedo,
                    scattered
                };
            }
            else
            {
                return std::nullopt;
            }
        }
    };


    std::optional<vec3f>
    refract
    (
        const unit3f& uv,
        const unit3f& normal,
        float ni
    )
    {
        const auto dt = dot(uv, normal);
        const auto discriminant = 1.0f - ni*ni*(1.0f-dt*dt);
        if (discriminant > 0)
        {
            return ni * (uv - normal * dt) - normal * sqrt(discriminant);
        }
        else
        {
            return std::nullopt;
        }
    }


    float
    calculate_fresnel_factor(float cosine, float ref_idx)
    {
        const float r0 = square
        (
            (1-ref_idx) / (1+ref_idx)
        );
        return r0 + (1-r0)*std::pow((1.0f - cosine), 5.0f);
    }


    struct dielectric_material : material
    {
        rgb albedo;
        float refractive_index;

        explicit dielectric_material
        (
            const rgb& aalbedo,
            float arefractive_index
        )
            : albedo(aalbedo)
            , refractive_index(arefractive_index)
        {
        }

        std::optional<scatter_result>
        scatter
        (
            const unit_ray3f& ray,
            const hit_result& hit,
            random* random
        ) override
        {
            const auto dr = dot(ray.dir, hit.normal);
            const auto dot_result = dr > 0.0f;

            const auto outward_normal = dot_result ? -hit.normal : hit.normal;
            const auto ni = dot_result ? refractive_index : 1.0f/refractive_index;

            const auto refracted = refract(ray.dir, outward_normal, ni);

            if(refracted.has_value())
            {
                const auto cosine = dot_result
                    ? refractive_index * dr
                    : -dr
                    ;
                const auto reflection_probability = calculate_fresnel_factor(cosine, refractive_index);
                if( random->get_next_float01() >= reflection_probability )
                {
                    return scatter_result
                    {
                        albedo,
                        unit_ray3f::from_to
                        (
                            hit.position,
                            refracted.value()
                        )
                    };
                }
            }
            const auto reflected = reflect
            (
                ray.dir,
                hit.normal
            );
            return scatter_result
            {
                albedo,
                unit_ray3f::from_to
                (
                    hit.position,
                    reflected
                )
            };
        }
    };


    std::shared_ptr<material>
    create_diffuse_material
    (
        const rgb& albedo
    )
    {
        return std::make_shared<diffuse_material>
        (
            albedo
        );
    }


    std::shared_ptr<material>
    create_metal_material
    (
        const rgb& albedo,
        float fuzz
    )
    {
        return std::make_shared<metal_material>
        (
            albedo,
            fuzz
        );
    }


    std::shared_ptr<material>
    create_dielectric_material
    (
        const rgb& albedo,
        float refractive_index
    )
    {
        return std::make_shared<dielectric_material>
        (
            albedo,
            refractive_index
        );
    }


    rgb
    get_color
    (
        const scene& scene,
        const unit_ray3f& ray,
        random* random,
        int depth
    )
    {
        const auto h = scene.hit
        (
            ray,
            make_range(0.001f, std::numeric_limits<float>::max())
        );
        if(h.has_value())
        {
            if(depth < 50)
            {
                auto scatter = h->material->scatter
                (
                    ray,
                    h.value(),
                    random
                );
                if(scatter.has_value())
                {
                    const auto c = get_color
                    (
                        scene,
                        scatter->scattered,
                        random,
                        depth + 1
                    );
                    return scatter->attenuation * c;
                }
                else
                {
                    return color::black;
                }
            }
            else
            {
                return color::black;
            }
        }
        const auto t = (ray.dir.y+1)/2.0f;
        return rgb_transform::Transform
        (
            rgb(1.0f, 1.0f, 1.0f),
            t,
            rgb(0.5f, 0.7f, 1.0f)
        );
    }


    struct camera
    {
        static camera create(const angle& vfov, float aspect)
        {
            const auto half_height = tan(vfov/2.0f);
            const auto half_width = aspect * half_height;

            const auto lower_left_corner = vec3f{-half_width, -half_height, -1.0};
            const auto horizontal = vec3f{2*half_width, 0.0f, 0.0f};
            const auto vertical = vec3f{0.0f, 2*half_height, 0.0f};
            const auto origin = vec3f{0.0f, 0.0f, 0.0f};

            return camera
            {
                lower_left_corner,
                horizontal,
                vertical,
                origin
            };
        }

        vec3f lower_left_corner;
        vec3f horizontal;
        vec3f vertical;
        vec3f origin;

        [[nodiscard]] unit_ray3f
        get_ray(float u, float v) const
        {
            return unit_ray3f::from_to(origin, lower_left_corner + u*horizontal + v*vertical);
        }
    };


    rgb
    gamma2_correct_color(rgb color)
    {
        return {sqrt(color.r), sqrt(color.g), sqrt(color.b)};
    }

    void
    raytrace(image* aimage, const raytracer::scene& scene, int number_of_samples)
    {
        image& img = *aimage;

        auto rand = random{};
        const auto aspect_ratio = static_cast<float>(img.width) / static_cast<float>(img.height);
        const auto camera = camera::create(angle::from_degrees(90), aspect_ratio);

        std::cout << "Rendering ";
        for(int y=0; y<img.height; y+=1)
        {
            for(int x=0; x<img.width; x+=1)
            {
                rgb color = color::black;
                for(int sample = 0; sample < number_of_samples; sample += 1)
                {
                    const auto u = (static_cast<float>(x) + rand.get_next_float01()) / static_cast<float>(img.width);
                    const auto v = (static_cast<float>(y) + rand.get_next_float01()) / static_cast<float>(img.height);
                    const auto ray = camera.get_ray(u, v);
                    const auto sample_color = get_color(scene, ray, &rand, 0);
                    color += sample_color;
                }
                color = color/static_cast<float>(number_of_samples);
                color = gamma2_correct_color(color);
                img.set_pixel(x,y, crgbi(color));
            }
        }

        std::cout << "Rendering done :)\n";
    }
}

