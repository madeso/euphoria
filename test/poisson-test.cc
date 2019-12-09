#include "core/argparse.h"
#include "core/dump.h"
#include "core/poisson.h"
#include "core/random.h"
#include "core/rect.h"
#include "core/image.h"
#include "core/image_draw.h"
#include "core/io.h"

using namespace euphoria::core;
using namespace euphoria::core::dump2d;

void
svg_dump()
{
    Random random;

    const auto area = Rectf::FromWidthHeight(100, 100);
    const auto radius = 5.0f;

    const auto points = PoissonSample(area, &random, radius);

    auto svg = Dumper{};

    for(auto p: points)
    {
        svg << Circle{p, radius}.Line(Color::Black);
    }
    svg.AddAxis();
    svg.Write("poisson.html", 800, 600);
}


void
png_dump()
{
    Random random;

    const auto image_size = 512;
    const float world_size = 100;
    const auto radius = 5.0f;
    const auto extra_images = 10;

    auto image = Image{};
    image.SetupNoAlphaSupport(image_size, image_size);
    const auto area = Rectf::FromWidthHeight(world_size, world_size);
    auto frames = argparse::FileOutput("poisson-frames");
    frames.CreateDirIfMissing();

    auto worker = PoissonWorker{area, &random, radius, 30};

    const auto world_to_image = image_size / world_size;

    auto write_image = [&]()
    {
        Clear(&image, Color::Black);
        for(int i=0; i<worker.samples.size(); i+=1)
        {
            const auto is_active = std::find(worker.active.begin(), worker.active.end(), i) != worker.active.end();
            DrawCircle(&image, is_active ? Color::Blue : Color::White, (worker.samples[i]*world_to_image).StaticCast<int>(), radius * world_to_image);
        }
        io::ChunkToFile(image.Write(ImageWriteFormat::PNG), frames.NextFile());
    };

    if(!frames.single)
    {
        for(int i=0; i<extra_images; i +=1)
        {
            write_image();
        }
    }

    while(!worker.IsDone())
    {
        worker.Step();

        if(!frames.single)
        {
            write_image();
        }
    }

    if(!frames.single)
    {
        for(int i=0; i<extra_images; i +=1)
        {
            write_image();
        }
    }

    write_image();
}


int
main(int argc, char* argv[])
{
    auto parser = argparse::Parser {"Poisson test"};

    auto psvg = parser.AddSubParser(
            "svg", "generate svg file", [&] {
                svg_dump();
            });

    auto ppng = parser.AddSubParser(
            "png", "generate svg file", [&] {
                png_dump();
            });

    const auto status = parser.Parse(argc, argv);
    if(status != argparse::ParseResult::Ok)
    {
        return -1;
    }


    return 0;
}

