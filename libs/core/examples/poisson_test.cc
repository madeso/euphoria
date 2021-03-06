#include "core/argparse.h"
#include "core/dump.h"
#include "core/poisson.h"
#include "core/random.h"
#include "core/rect.h"
#include "core/image.h"
#include "core/image_draw.h"
#include "core/io.h"

using namespace euphoria;
using namespace euphoria::core;
using namespace euphoria::core::dump2d;

void
svg_dump()
{
    core::random random;

    const auto area = rectf::from_width_height(100, 100);
    const auto radius = 5.0f;

    const auto points = poisson_sample(area, &random, radius*2, radius);

    auto svg = dumper{};

    for(auto p: points)
    {
        svg << circle{p, radius}.set_line_color(color::black);
    }
    svg.add_axis();
    svg.write("poisson.html", 800, 600);
}


void
png_dump(int extra_images)
{
    core::random random;

    const auto image_size = 512;
    const float world_size = 100;
    const auto radius = 5.0f;
    auto frames = argparse::file_output("poisson-frames/");

    auto result = image{};
    result.setup_no_alpha_support(image_size, image_size);
    frames.create_dir_if_missing();

    auto worker = poisson_worker
    {
        rectf::from_width_height(world_size, world_size),
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

        clear(&result, {color::black});
        for
        (
            int i=0;
            i < c_sizet_to_int(worker.samples.size());
            i += 1
        )
        {
            const auto is_active = std::find
            (
                worker.active.begin(),
                worker.active.end(),
                i
            ) != worker.active.end();
            const auto circle_color = is_active ? color::blue : color::white;
            const auto cp = worker.samples[i]*world_to_image;
            const auto circle_position = (cp).StaticCast<int>();
            const auto circle_radius = radius * world_to_image;
            draw_circle(&result, circle_color, circle_position, circle_radius);
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
            draw_arrow
            (
                &result,
                from * world_to_image,
                to * world_to_image,
                {color::pure_red},
                2
            );
        }
        io::chunk_to_file(result.write(image_write_format::png), frames.get_next_file());
        // svg.Write("poisson.html", 800, 600);
    };

    if(!frames.single)
    {
        for(int i=0; i<extra_images; i +=1)
        {
            write_image(std::nullopt);
        }
    }

    while(!worker.is_done())
    {
        auto r = worker.step();

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
    auto parser = argparse::parser {"Poisson test"};

    auto sub = parser.add_sub_parsers();

    sub->add
    (
        "svg", "generate svg file",
        [](argparse::sub_parser* sub)
        {
            return sub->on_complete
            (
                []
                {
                    svg_dump();
                    return argparse::ok;
                }
            );
        }
    );

    sub->add
    (
        "png", "generate png file",
        [](argparse::sub_parser* sub)
        {
            int extra_frames = 0;
            sub->add("--extra-frames", &extra_frames);

            return sub->on_complete
            (
                [&]
                {
                    png_dump(extra_frames);
                    return argparse::ok;
                }
            );
        }
    );

    return argparse::parse_from_main(&parser, argc, argv);
}

