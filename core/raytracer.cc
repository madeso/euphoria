#include "core/raytracer.h"

#include "core/image.h"
#include "core/vec3.h"
#include "core/ray.h"

#include "core/intersection.h"
#include "core/sphere.h"

namespace euphoria::core
{
    Rgb
    GetColor(const UnitRay3f& ray)
    {
        if
        (
            GetIntersection(ray, Sphere{0.5f}, vec3f{0.0f,0.0f,-1.0f}) > 0.0f
        )
        {
            return Color::Red;
        }
        const auto t = (ray.dir.y+1)/2.0f;
        return RgbTransform::Transform
        (
            Rgb(1.0f, 1.0f, 1.0f),
            t,
            Rgb(0.5f, 0.7f, 1.0f)
        );
    }

    void
    Raytrace(Image* aimage)
    {
        Image& img = *aimage;

        const auto lower_left_corner = vec3f{-2.0f, -1.0f, -1.0f};
        const auto horizontal = vec3f{4.0f, 0.0f, 0.0f};
        const auto vertical = vec3f{0.0f, 2.0f, 0.0f};
        const auto origin = vec3f{0.0f, 0.0f, 0.0f};

        for(int y=0; y<img.GetHeight(); y+=1)
        for(int x=0; x<img.GetWidth(); x+=1)
        {
            const auto u = x / static_cast<float>(img.GetWidth());
            const auto v = y / static_cast<float>(img.GetHeight());
            const auto ray = UnitRay3f::FromTo(origin, lower_left_corner + u*horizontal + v*vertical);
            const auto color = GetColor(ray);
            img.SetPixel(x,y, rgbi(color));
        }
    }
}

