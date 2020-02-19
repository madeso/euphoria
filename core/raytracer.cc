#include "core/raytracer.h"

#include "core/image.h"

#include "core/intersection.h"
#include "core/sphere.h"
#include "core/random.h"
#include "core/polarcoord.h"
#include "core/numeric.h"
#include "core/cli_progress_dots.h"

#include <limits>

namespace euphoria::core::raytracer
{
    HitResult::HitResult
    (
        bool acollided,
        float aray_distance,
        const vec3f& aposition,
        const unit3f& anormal
    )
        : collided(acollided)
        , ray_distance(aray_distance)
        , position(aposition)
        , normal(anormal)
    {
    }


    HitResult
    NoCollision
    (
    )
    {
        return HitResult
        {
            false,
            10000.0f,
            vec3f::Zero(),
            unit3f::Up()
        };
    }


    HitResult
    Collision
    (
        float ray_distance,
        const vec3f& position,
        const unit3f& normal
    )
    {
        return HitResult
        {
            true,
            ray_distance,
            position,
            normal
        };
    }



    struct SphereObject : public Object
    {
        Sphere sphere;
        vec3f position;

        SphereObject(const Sphere& asphere, const vec3f& aposition)
            : sphere(asphere)
            , position(aposition)
        {
        }

        HitResult
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
                return Collision
                (
                    hit_index,
                    hit_position,
                    hit_normal
                );
            }
            else
            {
                return NoCollision();
            }
        }
    };


    std::shared_ptr<Object>
    CreateSphere(const Sphere& sphere, const vec3f& position)
    {
        return std::make_shared<SphereObject>
        (
            sphere,
            position
        );
    }


    HitResult
    Scene::Hit(const UnitRay3f& ray, const Range<float>& range) const
    {
        auto r = NoCollision();

        for(const auto o: objects)
        {
            const auto h = o->Hit(ray, range);
            if(r.collided == false)
            {
                r = h;
            }
            else
            {
                if(h.collided)
                {
                    if(h.ray_distance < r.ray_distance)
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


    Rgb
    GetColor(const Scene& scene, const UnitRay3f& ray, Random* random)
    {
        const auto h = scene.Hit
        (
            ray,
            MakeRange(0.001f, std::numeric_limits<float>::max())
        );
        if(h.collided)
        {
            const auto target = h.position + h.normal + RandomInUnitSphere(random);
            const auto reflected_ray = UnitRay3f::FromTo(h.position, target);
            return 0.5f * GetColor(scene, reflected_ray, random);
            // return rgb(h.normal);
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

        Dots dots;

        for(int y=0; y<img.GetHeight(); y+=1)
        for(int x=0; x<img.GetWidth(); x+=1)
        {
            Rgb color = Color::Black;
            for(int sample = 0; sample < number_of_samples; sample += 1)
            {
                const auto u = (x + random.NextFloat01()) / static_cast<float>(img.GetWidth());
                const auto v = (y + random.NextFloat01()) / static_cast<float>(img.GetHeight());
                const auto ray = camera.GetRay(u, v);
                const auto sample_color = GetColor(scene, ray, &random);
                color += sample_color;
            }
            color = color/number_of_samples;
            color = Gamma2CorrectColor(color);
            img.SetPixel(x,y, rgbi(color));
            dots.Dot(80);
        }
    }
}

