#include <array>

#include "core/image.h"
#include "io/io.h"
#include "core/raytracer.h"
#include "core/argparse.h"
#include "core/timepoint.h"

using namespace eu;
using namespace eu::core;
using namespace eu::io;


int
main(int argc, char* argv[])
{
    auto image_width = 200;
    auto image_height = 100;
    int number_of_samples = 100;

    auto parser = argparse::Parser {"euphoria raytracer"};
    parser.add("-width", &image_width).set_help("image width");
    parser.add("-height", &image_height).set_help("image height");
    parser.add("-samples", &number_of_samples).set_help("number of samples (anitaliasing)");

    if(auto r = parser.parse(argc, argv))
    {
        return *r;
    }

    Image image;
    image.setup_no_alpha_support(image_width, image_height);

    raytracer::Scene scene;

    scene.objects.push_back
    (
        raytracer::create_sphere
        (
            Sphere{0.5f},
            vec3f(0.0f, 0.0f, -1.0f),
            raytracer::create_diffuse_material
            (
                Rgb(0.1f, 0.2f, 0.5f)
            )
        )
    );
    scene.objects.push_back
    (
        raytracer::create_sphere
        (
            Sphere{100.0f},
            vec3f(0.0f, -100.5f, -1.0f),
            raytracer::create_diffuse_material
            (
                Rgb(0.8f, 0.8f, 0.0f)
            )
        )
    );
    scene.objects.push_back
    (
        raytracer::create_sphere
        (
            Sphere{0.5f},
            vec3f(1.0f, 0.0f, -1.0f),
            raytracer::create_metal_material
            (
                Rgb(0.8f, 0.6f, 0.2f),
                0.3f
            )
        )
    );
    scene.objects.push_back
    (
        raytracer::create_sphere
        (
            Sphere{0.5f},
            vec3f(-1.0f, 0.0f, -1.0f),
            raytracer::create_dielectric_material
            (
                Rgb(1.0f, 1.0f, 1.0f),
                1.5f
            )
        )
    );

    const auto start = get_current_timepoint();
    raytracer::raytrace(&image, scene, number_of_samples);
    const auto end = get_current_timepoint();
    const auto seconds = get_seconds_between(start, end);
    std::cout << "Rendering took " << seconds << " seconds.\n";
    write_chunk_to_file(image.write(ImageWriteFormat::png), "raytracer.png");

    return 0;
}

