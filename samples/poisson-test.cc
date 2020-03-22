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

    const auto points = PoissonSample(area, &random, radius*2, radius);

    auto svg = Dumper{};

    for(auto p: points)
    {
        svg << Circle{p, radius}.Line(Color::Black);
    }
    svg.AddAxis();
    svg.Write("poisson.html", 800, 600);
}


void
png_dump(int extra_images)
{
    Random random;

    const auto image_size = 512;
    const float world_size = 100;
    const auto radius = 5.0f;
    auto frames = argparse::FileOutput("poisson-frames/");

    auto image = Image{};
    image.SetupNoAlphaSupport(image_size, image_size);
    frames.CreateDirIfMissing();

    auto worker = PoissonWorker
    {
        Rectf::FromWidthHeight(world_size, world_size),
        &random,
        radius*2,
        radius,
        30
    };

    const auto world_to_image = image_size / world_size;

    auto write_image = [&](std::optional<std::tuple<vec2f, vec2f>> line)
    {
        // auto svg = Dumper{};
        // svg.canvas_color = Color::Black;

        Clear(&image, Color::Black);
        for
        (
            int i=0;
            i < Csizet_to_int(worker.samples.size());
            i += 1
        )
        {
            const auto is_active = std::find
            (
                worker.active.begin(),
                worker.active.end(),
                i
            ) != worker.active.end();
            const auto circle_color = is_active ? Color::Blue : Color::White;
            const auto cp = worker.samples[i]*world_to_image;
            const auto circle_position = (cp).StaticCast<int>();
            const auto circle_radius = radius * world_to_image;
            DrawCircle(&image, circle_color, circle_position, circle_radius);
            // svg << Circle(circle_position.StaticCast<float>(),
            // circle_radius, circle_color);
        }
        if(line)
        {
            const auto [from, to] = *line;
            // DrawLineFast(&image, Color::PureRed,
            // (from*world_to_image).StaticCast<int>(),
            // (to*world_to_image).StaticCast<int>());
            // DrawCircle(&image, Color::PureRed,
            // (to*world_to_image).StaticCast<int>(), 10);
            // DrawCircle(&image, Color::PureYellow,
            // (from*world_to_image).StaticCast<int>(), 10);
            DrawArrow
            (
                &image,
                from * world_to_image,
                to * world_to_image,
                Color::PureRed,
                2
            );
        }
        io::ChunkToFile(image.Write(ImageWriteFormat::PNG), frames.NextFile());
        // svg.Write("poisson.html", 800, 600);
    };

    if(!frames.single)
    {
        for(int i=0; i<extra_images; i +=1)
        {
            write_image(std::nullopt);
        }
    }

    while(!worker.IsDone())
    {
        auto r = worker.Step();

        if(!frames.single)
        {
            write_image(r);
        }
    }

    if(!frames.single)
    {
        for(int i=0; i<extra_images; i +=1)
        {
            write_image(std::nullopt);
        }
    }

    write_image(std::nullopt);
}


int
main(int argc, char* argv[])
{
    auto parser = argparse::Parser {"Poisson test"};

    auto sub = parser.AddSubParsers();

    sub->Add
    (
        "svg", "generate svg file",
        [](argparse::SubParser* sub)
        {
            return sub->OnComplete
            (
                []
                {
                    svg_dump();
                    return argparse::ParseResult::Ok;
                }
            );
        }
    );

    sub->Add
    (
        "png", "generate png file",
        [](argparse::SubParser* sub)
        {
            int extra_frames = 0;
            sub->Add("--extra-frames", &extra_frames);

            return sub->OnComplete
            (
                [&]
                {
                    png_dump(extra_frames);
                    return argparse::ParseResult::Ok;
                }
            );
        }
    );

    return argparse::ParseFromMain(&parser, argc, argv);
}

