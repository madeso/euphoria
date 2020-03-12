#include <array>

#include "core/image.h"
#include "core/io.h"
#include "core/raytracer.h"
#include "core/argparse.h"
#include "core/timepoint.h"

using namespace euphoria::core;

int
main(int argc, char* argv[])
{
    auto image_width = 200;
    auto image_height = 100;
    int number_of_samples = 100;

    auto parser = argparse::Parser {"euphoria raytracer"};
    parser.Add("-width", &image_width).Help("image width");
    parser.Add("-height", &image_height).Help("image height");
    parser.Add("-samples", &number_of_samples).Help("number of samples (anitaliasing)");

    if(auto r = parser.Parse(argc, argv))
    {
        return *r;
    }

    Image image;
    image.SetupNoAlphaSupport(image_width, image_height);

    raytracer::Scene scene;

    scene.objects.push_back
    (
        raytracer::CreateSphere
        (
            Sphere{0.5f},
            vec3f(0.0f, 0.0f, -1.0f),
            raytracer::CreateDiffuseMaterial
            (
                Rgb(0.1f, 0.2f, 0.5f)
            )
        )
    );
    scene.objects.push_back
    (
        raytracer::CreateSphere
        (
            Sphere{100.0f},
            vec3f(0.0f, -100.5f, -1.0f),
            raytracer::CreateDiffuseMaterial
            (
                Rgb(0.8f, 0.8f, 0.0f)
            )
        )
    );
    scene.objects.push_back
    (
        raytracer::CreateSphere
        (
            Sphere{0.5f},
            vec3f(1.0f, 0.0f, -1.0f),
            raytracer::CreateMetalMaterial
            (
                Rgb(0.8f, 0.6f, 0.2f),
                0.3f
            )
        )
    );
    scene.objects.push_back
    (
        raytracer::CreateSphere
        (
            Sphere{0.5f},
            vec3f(-1.0f, 0.0f, -1.0f),
            raytracer::CreateDielectricMaterial
            (
                Rgb(1.0f, 1.0f, 1.0f),
                1.5f
            )
        )
    );

    const auto start = Now();
    raytracer::Raytrace(&image, scene, number_of_samples);
    const auto end = Now();
    const auto seconds = SecondsBetween(start, end);
    std::cout << "Rendering took " << seconds << " seconds.\n";
    io::ChunkToFile(image.Write(ImageWriteFormat::PNG), "raytracer.png");

    return 0;
}

