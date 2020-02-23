#include "core/raytracer.h"

#include "core/image.h"

#include "core/intersection.h"
#include "core/sphere.h"
#include "core/random.h"
#include "core/polarcoord.h"
#include "core/numeric.h"
#include "core/cli_progress_bar.h"

#include <limits>

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
        Sphere sphere;
        vec3f position;
        std::shared_ptr<Material> material;

        SphereObject
        (
            const Sphere& asphere,
            const vec3f& aposition,
            std::shared_ptr<Material> amaterial
        )
            : sphere(asphere)
            , position(aposition)
            , material(amaterial)
        {
        }

        std::optional<HitResult>
        Hit(const UnitRay3f& ray, const Range<float>& range) const override
        {
            const auto hit_index = GetIntersection
            (
                ray,
                sphere,
                position
            );
            if(IsWithin(range, hit_index))
            {
                const auto hit_position = ray.GetPoint(hit_index);
                const auto hit_normal = vec3f::FromTo(position, hit_position).GetNormalized();
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
        const Sphere& sphere,
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
    Scene::Hit(const UnitRay3f& ray, const Range<float>& range) const
    {
        std::optional<HitResult> r = std::nullopt;

        for(const auto o: objects)
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


    Rgb
    rgb(const unit3f& normal)
    {
        return Rgb
        {
            (normal.x + 1)/2,
            (normal.y + 1)/2,
            (normal.z + 1)/2
        };
    }


    vec3f RandomInUnitSphere(Random* random)
    {
        return RandomUnit3(random) * random->NextFloat01();
    }


    Material::~Material() {}

    struct DiffuseMaterial : public Material
    {
        Rgb albedo;

        explicit DiffuseMaterial(const Rgb& aalbedo)
            : albedo(aalbedo)
        {
        }

        std::optional<ScatterResult>
        Scatter
        (
            const UnitRay3f& /*ray*/,
            const HitResult& hit,
            Random* random
        ) override
        {
            const auto target = hit.position + hit.normal + RandomInUnitSphere(random);
            const auto reflected_ray = UnitRay3f::FromTo(hit.position, target);

            return ScatterResult
            {
                albedo,
                reflected_ray
            };
        }
    };


    std::shared_ptr<Material>
    CreateDiffuseMaterial
    (
        const Rgb& albedo
    )
    {
        return std::make_shared<DiffuseMaterial>(albedo);
    }


    Rgb
    GetColor
    (
        const Scene& scene,
        const UnitRay3f& ray,
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
                    return Color::Black;
                }
            }
            else
            {
                return Color::Black;
            }
        }
        const auto t = (ray.dir.y+1)/2.0f;
        return RgbTransform::Transform
        (
            Rgb(1.0f, 1.0f, 1.0f),
            t,
            Rgb(0.5f, 0.7f, 1.0f)
        );
    }


    struct Camera
    {
        vec3f lower_left_corner = vec3f{-2.0f, -1.0f, -1.0f};
        vec3f horizontal = vec3f{4.0f, 0.0f, 0.0f};
        vec3f vertical = vec3f{0.0f, 2.0f, 0.0f};
        vec3f origin = vec3f{0.0f, 0.0f, 0.0f};

        UnitRay3f GetRay(float u, float v) const
        {
            return UnitRay3f::FromTo(origin, lower_left_corner + u*horizontal + v*vertical);
        }
    };


    Rgb
    Gamma2CorrectColor(Rgb color)
    {
        return {Sqrt(color.r), Sqrt(color.g), Sqrt(color.b)};
    }

    void
    Raytrace(Image* aimage, const Scene& scene, int number_of_samples)
    {
        Image& img = *aimage;

        auto random = Random{};
        const auto camera = Camera{};

        {
        std::cout << "Rendering ";
        CliProgressBar progress;

        const int number_of_pixels = img.GetWidth() * img.GetHeight();

        for(int y=0; y<img.GetHeight(); y+=1)
        for(int x=0; x<img.GetWidth(); x+=1)
        {
            Rgb color = Color::Black;
            for(int sample = 0; sample < number_of_samples; sample += 1)
            {
                const auto u = (x + random.NextFloat01()) / static_cast<float>(img.GetWidth());
                const auto v = (y + random.NextFloat01()) / static_cast<float>(img.GetHeight());
                const auto ray = camera.GetRay(u, v);
                const auto sample_color = GetColor(scene, ray, &random, 0);
                color += sample_color;
            }
            color = color/number_of_samples;
            color = Gamma2CorrectColor(color);
            img.SetPixel(x,y, rgbi(color));
            progress.Update( (y*img.GetWidth() + x)/static_cast<float>(number_of_pixels) );
        }
        }
        std::cout << "Rendering done :)\n";
    }
}

