#include "core/raytracer.h"

#include "core/image.h"

#include "core/intersection.h"
#include "core/sphere.h"
#include "base/random.h"
#include "base/polarcoord.h"
#include "base/numeric.h"
#include "base/angle.h"

#include <limits>
#include <cmath>

namespace eu::core::raytracer
{
    Collision::Collision
    (
        float aray_distance,
        const vec3f& aposition,
        const unit3f& anormal,
        std::shared_ptr<raytracer::Material> amaterial
    )
        : ray_distance(aray_distance)
        , position(aposition)
        , normal(anormal)
        , material(amaterial)
    {
    }


    struct SphereObject : Object
    {
        core::Sphere sphere;
        vec3f position;
        std::shared_ptr<raytracer::Material> material;

        SphereObject
        (
            const core::Sphere& asphere,
            const vec3f& aposition,
            std::shared_ptr<raytracer::Material> amaterial
        )
            : sphere(asphere)
            , position(aposition)
            , material(amaterial)
        {
        }

        [[nodiscard]] std::optional<Collision>
        get_collision(const UnitRay3f& ray, const Range<float>& range) const override
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
                return Collision
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


    std::shared_ptr<Object>
    create_sphere
    (
        const Sphere& sphere,
        const vec3f& position,
        std::shared_ptr<raytracer::Material> material
    )
    {
        return std::make_shared<SphereObject>
        (
            sphere,
            position,
            material
        );
    }


    std::optional<Collision>
    Scene::get_collision(const UnitRay3f& ray, const Range<float>& range) const
    {
        std::optional<Collision> r = std::nullopt;

        for(const auto& o: objects)
        {
            const auto h = o->get_collision(ray, range);
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


    Rgb
    to_rgb(const unit3f& normal)
    {
        return Rgb
        {
            (normal.x + 1)/2,
            (normal.y + 1)/2,
            (normal.z + 1)/2
        };
    }


    vec3f get_random_vec3_in_unit_sphere(Random* random)
    {
        return get_random_unit3(random) * random->get_next_float01();
    }


    struct DiffuseMaterial : public Material
    {
        Rgb albedo;

        explicit DiffuseMaterial(const Rgb& aalbedo)
            : albedo(aalbedo)
        {
        }

        std::optional<ScatterResult>
        get_scattered
        (
            const UnitRay3f& /*ray*/,
            const Collision& hit,
            Random* random
        ) override
        {
            const auto target = hit.position + hit.normal + get_random_vec3_in_unit_sphere(random);
            const auto reflected_ray = UnitRay3f::from_to(hit.position, target);

            return ScatterResult
            {
                albedo,
                reflected_ray
            };
        }
    };


    vec3f get_reflected(const vec3f& v, const unit3f& normal) {
        return v - 2 * v.dot(normal) * normal;
    }


    struct MetalMaterial : Material
    {
        Rgb albedo;
        float fuzz;

        explicit MetalMaterial
        (
            const Rgb& aalbedo,
            float afuzz
        )
            : albedo(aalbedo)
            , fuzz(keep_within(r01, afuzz))
        {
        }

        std::optional<ScatterResult>
        get_scattered
        (
            const UnitRay3f& ray,
            const Collision& hit,
            Random* random
        ) override
        {
            const auto reflected = get_reflected
            (
                ray.dir,
                hit.normal
            );
            const auto scattered = UnitRay3f::from_to
            (
                hit.position,
                reflected + fuzz * get_random_vec3_in_unit_sphere(random)
            );
            const auto scatter_dot = scattered.dir.dot(hit.normal);
            if(scatter_dot > 0)
            {
                return ScatterResult
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
    get_refracted
    (
        const unit3f& uv,
        const unit3f& normal,
        float ni
    )
    {
        const auto dt = uv.dot(normal);
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
    calc_fresnel_factor(float cosine, float ref_idx)
    {
        const float r0 = square
        (
            (1-ref_idx) / (1+ref_idx)
        );
        return r0 + (1-r0)*std::pow((1.0f - cosine), 5.0f);
    }


    struct DielectricMaterial : Material
    {
        Rgb albedo;
        float refractive_index;

        explicit DielectricMaterial
        (
            const Rgb& aalbedo,
            float arefractive_index
        )
            : albedo(aalbedo)
            , refractive_index(arefractive_index)
        {
        }

        std::optional<ScatterResult>
        get_scattered
        (
            const UnitRay3f& ray,
            const Collision& hit,
            Random* random
        ) override
        {
            const auto dr = ray.dir.dot(hit.normal);
            const auto dot_result = dr > 0.0f;

            const auto outward_normal = dot_result ? -hit.normal : hit.normal;
            const auto ni = dot_result ? refractive_index : 1.0f/refractive_index;

            const auto refracted = get_refracted(ray.dir, outward_normal, ni);

            if(refracted.has_value())
            {
                const auto cosine = dot_result
                    ? refractive_index * dr
                    : -dr
                    ;
                const auto reflection_probability = calc_fresnel_factor(cosine, refractive_index);
                if( random->get_next_float01() >= reflection_probability )
                {
                    return ScatterResult
                    {
                        albedo,
                        UnitRay3f::from_to
                        (
                            hit.position,
                            refracted.value()
                        )
                    };
                }
            }
            const auto reflected = get_reflected
            (
                ray.dir,
                hit.normal
            );
            return ScatterResult
            {
                albedo,
                UnitRay3f::from_to
                (
                    hit.position,
                    reflected
                )
            };
        }
    };


    std::shared_ptr<Material>
    create_diffuse_material
    (
        const Rgb& albedo
    )
    {
        return std::make_shared<DiffuseMaterial>
        (
            albedo
        );
    }


    std::shared_ptr<Material>
    create_metal_material
    (
        const Rgb& albedo,
        float fuzz
    )
    {
        return std::make_shared<MetalMaterial>
        (
            albedo,
            fuzz
        );
    }


    std::shared_ptr<Material>
    create_dielectric_material
    (
        const Rgb& albedo,
        float refractive_index
    )
    {
        return std::make_shared<DielectricMaterial>
        (
            albedo,
            refractive_index
        );
    }


    Rgb
    get_color
    (
        const Scene& scene,
        const UnitRay3f& ray,
        Random* random,
        int depth
    )
    {
        const auto h = scene.get_collision
        (
            ray,
            make_range(0.001f, std::numeric_limits<float>::max())
        );
        if(h.has_value())
        {
            if(depth < 50)
            {
                auto scatter = h->material->get_scattered
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
                    return NamedColor::black;
                }
            }
            else
            {
                return NamedColor::black;
            }
        }
        const auto t = (ray.dir.y+1)/2.0f;
        return lerp_rgb
        (
            Rgb(1.0f, 1.0f, 1.0f),
            t,
            Rgb(0.5f, 0.7f, 1.0f)
        );
    }


    struct Camera
    {
        vec3f lower_left_corner;
        vec3f horizontal;
        vec3f vertical;
        vec3f origin;

        static Camera create(const Angle& vfov, float aspect)
        {
            const auto half_height = tan(vfov/2.0f);
            const auto half_width = aspect * half_height;

            const auto lower_left_corner = vec3f{-half_width, -half_height, -1.0};
            const auto horizontal = vec3f{2*half_width, 0.0f, 0.0f};
            const auto vertical = vec3f{0.0f, 2*half_height, 0.0f};
            const auto origin = vec3f{0.0f, 0.0f, 0.0f};

            return Camera
            {
                lower_left_corner,
                horizontal,
                vertical,
                origin
            };
        }

        [[nodiscard]] UnitRay3f
        get_ray(float u, float v) const
        {
            return UnitRay3f::from_to(origin, lower_left_corner + u*horizontal + v*vertical);
        }
    };


    Rgb
    correct_color_using_gamma2(const Rgb& color)
    {
        return {sqrt(color.r), sqrt(color.g), sqrt(color.b)};
    }

    void
    raytrace(Image* aimage, const raytracer::Scene& scene, int number_of_samples)
    {
        Image& img = *aimage;

        auto rand = Random{};
        const auto aspect_ratio = static_cast<float>(img.width) / static_cast<float>(img.height);
        const auto camera = Camera::create(Angle::from_degrees(90), aspect_ratio);

        std::cout << "Rendering ";
        for(int y=0; y<img.height; y+=1)
        {
            for(int x=0; x<img.width; x+=1)
            {
                Rgb color = NamedColor::black;
                for(int sample = 0; sample < number_of_samples; sample += 1)
                {
                    const auto u = (static_cast<float>(x) + rand.get_next_float01()) / static_cast<float>(img.width);
                    const auto v = (static_cast<float>(y) + rand.get_next_float01()) / static_cast<float>(img.height);
                    const auto ray = camera.get_ray(u, v);
                    const auto sample_color = get_color(scene, ray, &rand, 0);
                    color += sample_color;
                }
                color = color/static_cast<float>(number_of_samples);
                color = correct_color_using_gamma2(color);
                img.set_pixel(x,y, to_rgbi(color));
            }
        }

        std::cout << "Rendering done :)\n";
    }
}

