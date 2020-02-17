#include <array>

#include "core/image.h"
#include "core/io.h"
#include "core/raytracer.h"
#include "core/argparse.h"

using namespace euphoria::core;

int
main(int argc, char* argv[])
{
    auto image_width = 200;
    auto image_height = 100;

    auto parser = argparse::Parser {"euphoria raytracer"};
    parser.AddSimple("-width", &image_width).Help("image width");
    parser.AddSimple("-height", &image_height).Help("image height");

    if(parser.Parse(argc, argv) != argparse::ParseResult::Ok)
    {
        return -1;
    }

    Image image;
    image.SetupNoAlphaSupport(image_width, image_height);

    raytracer::Scene scene;
    scene.objects.push_back
    (
        raytracer::CreateSphere
        (
            Sphere{0.5f},
            vec3f(0.0f, 0.0f, -1.0f)
        )
    );
    scene.objects.push_back
    (
        raytracer::CreateSphere
        (
            Sphere{100.0f},
            vec3f(0.0f, -100.5f, -1.0f)
        )
    );

    raytracer::Raytrace(&image, scene);
    io::ChunkToFile(image.Write(ImageWriteFormat::PNG), "raytracer.png");

    return 0;
}

