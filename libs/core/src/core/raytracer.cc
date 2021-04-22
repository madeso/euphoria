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
    HitResult::HitResult
    (
        float aray_distance,
        const vec3f& aposition,
        const unit3f& anormal,
        std::shared_ptr<Material> amaterial
    )
        : ray_distance(aray_distance)
        , position(aposition)
        , normal(anormal)
        , material(amaterial)
    {
    }


    struct SphereObject : public Object
    {
        core::sphere sphere;
        vec3f position;
        std::shared_ptr<Material> material;

        SphereObject
        (
            const core::sphere& asphere,
            const vec3f& aposition,
            std::shared_ptr<Material> amaterial
        )
            : sphere(asphere)
            , position(aposition)
            , material(amaterial)
        {
        }

        [[nodiscard]] std::optional<HitResult>
        Hit(const unit_ray3f& ray, const Range<float>& range) const override
        {
            const auto hit_index = GetIntersection
            (
                ray,
                sphere,
                position
            );
            if(IsWithin(range, hit_index))
            {
                const auto hit_position = ray.get_point(hit_index);
                const auto hit_normal = vec3f::from_to(position, hit_position).get_normalized();
                return HitResult
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
    CreateSphere
    (
        const sphere& sphere,
        const vec3f& position,
        std::shared_ptr<Material> material
    )
    {
        return std::make_shared<SphereObject>
        (
            sphere,
            position,
            material
        );
    }


    std::optional<HitResult>
    Scene::Hit(const unit_ray3f& ray, const Range<float>& range) const
    {
        std::optional<HitResult> r = std::nullopt;

        for(const auto& o: objects)
        {
            const auto h = o->Hit(ray, range);
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


    vec3f RandomInUnitSphere(Random* random)
    {
        return create_random_unit3(random) * random->NextFloat01();
    }


    struct DiffuseMaterial : public Material
    {
        rgb albedo;

        explicit DiffuseMaterial(const rgb& aalbedo)
            : albedo(aalbedo)
        {
        }

        std::optional<ScatterResult>
        Scatter
        (
            const unit_ray3f& /*ray*/,
            const HitResult& hit,
            Random* random
        ) override
        {
            const auto target = hit.position + hit.normal + RandomInUnitSphere(random);
            const auto reflected_ray = unit_ray3f::from_to(hit.position, target);

            return ScatterResult
            {
                albedo,
                reflected_ray
            };
        }
    };


    vec3f Reflect(const vec3f& v, const unit3f& n) {
        return v - 2*dot(v,n)*n;
    }


    struct MetalMaterial : public Material
    {
        rgb albedo;
        float fuzz;

        explicit MetalMaterial
        (
            const rgb& aalbedo,
            float afuzz
        )
            : albedo(aalbedo)
            , fuzz(KeepWithin(R01(), afuzz))
        {
        }

        std::optional<ScatterResult>
        Scatter
        (
            const unit_ray3f& ray,
            const HitResult& hit,
            Random* random
        ) override
        {
            const auto reflected = Reflect
            (
                ray.dir,
                hit.normal
            );
            const auto scattered = unit_ray3f::from_to
            (
                hit.position,
                reflected + fuzz * RandomInUnitSphere(random)
            );
            const auto scatter_dot = dot
            (
                scattered.dir,
                hit.normal
            );
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
    Refract
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
            return ni * (uv - normal * dt) - normal * Sqrt(discriminant);
        }
        else
        {
            return std::nullopt;
        }
    }


    float
    FresnelFactor(float cosine, float ref_idx)
    {
        const float r0 = Square
        (
            (1-ref_idx) / (1+ref_idx)
        );
        return r0 + (1-r0)*std::pow((1.0f - cosine), 5.0f);
    }


    struct DielectricMaterial : public Material
    {
        rgb albedo;
        float refractive_index;

        explicit DielectricMaterial
        (
            const rgb& aalbedo,
            float arefractive_index
        )
            : albedo(aalbedo)
            , refractive_index(arefractive_index)
        {
        }

        std::optional<ScatterResult>
        Scatter
        (
            const unit_ray3f& ray,
            const HitResult& hit,
            Random* random
        ) override
        {
            const auto dr = dot(ray.dir, hit.normal);
            const auto dot_result = dr > 0.0f;

            const auto outward_normal = dot_result ? -hit.normal : hit.normal;
            const auto ni = dot_result ? refractive_index : 1.0f/refractive_index;

            const auto refracted = Refract(ray.dir, outward_normal, ni);

            if(refracted.has_value())
            {
                const auto cosine = dot_result
                    ? refractive_index * dr
                    : -dr
                    ;
                const auto reflection_probability = FresnelFactor(cosine, refractive_index);
                if( random->NextFloat01() >= reflection_probability )
                {
                    return ScatterResult
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
            const auto reflected = Reflect
            (
                ray.dir,
                hit.normal
            );
            return ScatterResult
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


    std::shared_ptr<Material>
    CreateDiffuseMaterial
    (
        const rgb& albedo
    )
    {
        return std::make_shared<DiffuseMaterial>
        (
            albedo
        );
    }


    std::shared_ptr<Material>
    CreateMetalMaterial
    (
        const rgb& albedo,
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
    CreateDielectricMaterial
    (
        const rgb& albedo,
        float refractive_index
    )
    {
        return std::make_shared<DielectricMaterial>
        (
            albedo,
            refractive_index
        );
    }


    rgb
    GetColor
    (
        const Scene& scene,
        const unit_ray3f& ray,
        Random* random,
        int depth
    )
    {
        const auto h = scene.Hit
        (
            ray,
            MakeRange(0.001f, std::numeric_limits<float>::max())
        );
        if(h.has_value())
        {
            if(depth < 50)
            {
                auto scatter = h->material->Scatter
                (
                    ray,
                    h.value(),
                    random
                );
                if(scatter.has_value())
                {
                    const auto c = GetColor
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


    struct Camera
    {
        static Camera Create(const angle& vfov, float aspect)
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

        vec3f lower_left_corner;
        vec3f horizontal;
        vec3f vertical;
        vec3f origin;

        [[nodiscard]] unit_ray3f
        GetRay(float u, float v) const
        {
            return unit_ray3f::from_to(origin, lower_left_corner + u*horizontal + v*vertical);
        }
    };


    rgb
    Gamma2CorrectColor(rgb color)
    {
        return {Sqrt(color.r), Sqrt(color.g), Sqrt(color.b)};
    }

    void
    Raytrace(image* aimage, const Scene& scene, int number_of_samples)
    {
        image& img = *aimage;

        auto random = Random{};
        const auto aspect_ratio = static_cast<float>(img.width) / static_cast<float>(img.height);
        const auto camera = Camera::Create(angle::from_degrees(90), aspect_ratio);

        std::cout << "Rendering ";
        for(int y=0; y<img.height; y+=1)
        {
            for(int x=0; x<img.width; x+=1)
            {
                rgb color = color::black;
                for(int sample = 0; sample < number_of_samples; sample += 1)
                {
                    const auto u = (static_cast<float>(x) + random.NextFloat01()) / static_cast<float>(img.width);
                    const auto v = (static_cast<float>(y) + random.NextFloat01()) / static_cast<float>(img.height);
                    const auto ray = camera.GetRay(u, v);
                    const auto sample_color = GetColor(scene, ray, &random, 0);
                    color += sample_color;
                }
                color = color/static_cast<float>(number_of_samples);
                color = Gamma2CorrectColor(color);
                img.set_pixel(x,y, crgbi(color));
            }
        }

        std::cout << "Rendering done :)\n";
    }
}

