#include "core/argparse.h"

#include <iostream>

#include <iomanip>
#include <algorithm>

#include "core/generator_maze.h"
#include "core/generator_cell.h"
#include "core/random.h"
#include "core/image_to_text.h"
#include "core/stringutils.h"
#include "core/io.h"
#include "core/knuthshuffle.h"

// later when doing floodfill
// #include "core/colorbrewer.h"


using namespace euphoria;
using namespace euphoria::core;


struct Difference
{
    int x;
    int y;
    bool new_value;
};


std::vector<Difference>
find_differences(const Table<bool>& src, const Table<bool>& dst)
{
    std::vector<Difference> ret;
    for(int y = 0; y < src.get_height(); y += 1)
    {
        for(int x = 0; x < src.get_width(); x += 1)
        {
            const auto lhs = src(x, y);
            const auto rhs = dst(x, y);
            if(lhs != rhs)
            {
                ret.push_back({x, y, lhs});
            }
        }
    }
    return ret;
}


void
print_maze_to_console(const generator::Drawer& drawer)
{
    const auto table = image_to_string_table
    (
        drawer.image,
        {
            {'#', drawer.wall_color},
            {'/', drawer.cell_color},
            {' ', drawer.wall_color},
            {' ', drawer.cell_visited_color},
            {'O', drawer.unit_color}
        }
    );

    const auto strings = to_strings(table);
    for(const auto& s: strings)
    {
        std::cout << s << "\n";
    }
}


enum class MazeAlgorithm
{
    recursive_backtracker,
    random_traversal
};


void
handle_maze_command
(
    MazeAlgorithm algo,
    int world_width,
    int world_height,
    int cell_size,
    int wall_size,
    const std::string& f,
    bool console
)
{
    auto output = argparse::FileOutput {f};
    auto rand = core::Random{};
    auto maze = generator::Maze::from_width_height(world_width, world_height);

    auto drawer = generator::Drawer {};

    std::unique_ptr<generator::Algorithm> gen;

    switch(algo)
    {
    case MazeAlgorithm::recursive_backtracker:
        {
            auto g = std::make_unique<generator::RecursiveBacktracker>();
            g->maze = &maze;
            g->random = &rand;
            drawer.tracker = g.get();
            gen = std::move(g);
        }
        break;
    case MazeAlgorithm::random_traversal:
        {
            auto g = std::make_unique<generator::RandomTraversal>();
            g->maze = &maze;
            g->random = &rand;
            drawer.traversal = g.get();
            gen = std::move(g);
        }
        break;
    default:
        DIE("Unhandled");
    }

    gen->setup();

    drawer.maze = &maze;
    drawer.cell_size = cell_size;
    drawer.wall_size = wall_size;

    auto draw_frame = [&]()
    {
        if(!output.single)
        {
            drawer.draw();
            io::chunk_to_file
            (
                drawer.image.write(ImageWriteFormat::png),
                output.get_next_file()
            );
        }
    };

    draw_frame();

    while(gen->has_more_work())
    {
        gen->work();
        draw_frame();
    }

    drawer.draw();

    if(console)
    {
        print_maze_to_console(drawer);
    }
    else
    {
        if(output.single)
        {
            io::chunk_to_file
            (
                drawer.image.write(ImageWriteFormat::png),
                output.file
            );
        }
        else
        {
            for(int i = 0; i < 5; i += 1)
            {
                io::chunk_to_file
                (
                    drawer.image.write(ImageWriteFormat::png),
                    output.get_next_file()
                );
            }
        }
    }
}



struct CellWriter
{
    bool debug;
    argparse::FileOutput output;
    generator::World* world;
    int world_scale;

    core::Random shuffle_random;
    generator::World world_copy;

    explicit CellWriter
    (
        bool d,
        const std::string& f,
        generator::World* w,
        int ws
    )
        : debug(d)
        , output{f}
        , world(w)
        , world_scale(ws)
    {
    }

    [[nodiscard]] Image
    generate_world_image(const generator::World& world_or_copy) const
    {
        return draw
        (
            world_or_copy,
            {NamedColor::black},
            {NamedColor::white},
            world_scale,
            std::nullopt
        );
    }

    void
    first_state()
    {
        if(!output.single)
        {
            auto img = generate_world_image(*world);
            io::chunk_to_file(img.write(ImageWriteFormat::png), output.get_next_file());
        }

        world_copy = *world;
        draw_step();
    }

    void
    done()
    {
        if(output.single)
        {
            auto img = generate_world_image(*world);
            io::chunk_to_file(img.write(ImageWriteFormat::png), output.file);
        }
        else
        {
            draw_step();
        }
    }

    void
    draw_step()
    {
        if(output.single) { return; }
        if (debug)
        {
            const auto img = generate_world_image(*world);
            io::chunk_to_file
            (
                img.write(ImageWriteFormat::png),
                output.get_next_file()
            );
        }
        else
        {
            shuffle_draw();
        }
    }

    void
    shuffle_draw()
    {
        auto diffs = find_differences(*world, world_copy);
        knuth_shuffle(&diffs, &shuffle_random);
        const auto diffs_per_write = std::max<decltype(diffs.size())>
        (
            2,
            diffs.size() / 25
        );
        int write_index = 0;
        for (const auto d : diffs)
        {
            world_copy(d.x, d.y) = d.new_value;
            if ((write_index % diffs_per_write) == 0)
            {
                const auto img = generate_world_image(world_copy);
                io::chunk_to_file
                (
                    img.write(ImageWriteFormat::png),
                    output.get_next_file()
                );
            }
            write_index += 1;
        }

        for (int i = 0; i < 5; i += 1)
        {
            const auto img = generate_world_image(world_copy);
            io::chunk_to_file
            (
                img.write(ImageWriteFormat::png),
                output.get_next_file()
            );
        }
    }
};


struct MazeArguments
{
    Size2i size = Size2i::create_from_width_height(10, 10);
    std::string output = "maze.png";

    int cell_size = 1;
    int wall_size = 1;
    bool console = false;

    void
    add(argparse::ParserBase* base)
    {
        base->add("--size", &size).set_help("set the size");
        base->add("-o, --output", &output).set_help("specify output");

        base->add("--cell", &cell_size).set_help("set the cell size");
        base->add("--wall", &wall_size).set_help("set the wall size");
        base->set_true("-c,--console", &console).set_help("foce console");
    }
};


constexpr NeighborhoodAlgorithm default_algorithm = NeighborhoodAlgorithm::box;

int
main(int argc, char* argv[])
{
    auto parser = argparse::Parser{"Generate worlds"};
    auto subs = parser.add_sub_parsers();

    subs->add
    (
        "recursive",
        "maze generation using recursive backtracker algorithm",
        [&](argparse::SubParser* sub)
        {
            auto args = MazeArguments{};
            args.add(sub);

            return sub->on_complete
            (
                [&]
                {
                    handle_maze_command
                    (
                        MazeAlgorithm::recursive_backtracker,
                        args.size.width,
                        args.size.height,
                        args.cell_size,
                        args.wall_size,
                        args.output,
                        args.console
                    );
                    return argparse::ok;
                }
            );
        }
    );

    subs->add
    (
        "random",
        "maze generation using random traversal algorithm",
        [&](argparse::SubParser* sub)
        {
            auto args = MazeArguments{};
            args.add(sub);

            return sub->on_complete
            (
                [&]
                {
                    handle_maze_command
                    (
                        MazeAlgorithm::random_traversal,
                        args.size.width,
                        args.size.height,
                        args.cell_size,
                        args.wall_size,
                        args.output,
                        args.console
                    );
                    return argparse::ok;
                }
            );
        }
    );

    subs->add
    (
        "cell",
        "world generation using cellular automata algorithm",
        [&](argparse::SubParser* sub)
        {
            int world_scale = 5;
            bool debug = false;
            Size2i size = Size2i::create_from_width_height(100, 70);
            std::string output = "cell.png";
            auto rand = core::Random{};
            auto rules = generator::Rules{};

            sub->add("--size", &size).set_help("set the size");
            sub->add("-o, --output", &output).set_help("specify output");
            sub->add("--scale", &world_scale).set_help("set the scale");
            sub->set_true("--debug", &debug);

            auto commands = sub->add_sub_parsers("commands");

            commands->add("random", "random fill", [&](argparse::SubParser* cmd)
            {
                cmd->parser_style = argparse::SubParserStyle::fallback;
                Lrud<BorderSetupRule> border_control = Lrud{BorderSetupRule::always_wall};
                float random_fill = 0.5;
                cmd->add("--fill", &random_fill).set_help("How much to fill");
                cmd->add("-bc", &border_control)
                    .set_help("Change how the border is generated")
                    ;
                return cmd->on_complete([&]{
                    generator::add_random_fill(&rules, &rand, random_fill, border_control);
                    return argparse::ok;
                });
            });
            commands->add("combo", "smooth map with combo rule", [&](argparse::SubParser* cmd)
            {
                cmd->parser_style = argparse::SubParserStyle::fallback;
                int times = 4;
                int count = 5;
                int small = 2;
                auto algorithm = default_algorithm;
                bool include_self = false;
                cmd->add("--algorithm", &algorithm).set_help("The algorithm to use");
                cmd->set_true("-i", &include_self).set_help("also include the current cell when calculating the amount of nodes");
                cmd->add("--times", &times).set_help("How many to run");
                cmd->add("--count", &count).set_help("neighbour count");
                cmd->add("--small", &small).set_help("small rule");
                return cmd->on_complete([&]{
                    generator::add_combo_rules(&rules, times, count, small, include_self, algorithm);
                    return argparse::ok;
                });
            });
            commands->add("spiky", "smooth map with spikes", [&](argparse::SubParser* cmd)
            {
                cmd->parser_style = argparse::SubParserStyle::fallback;
                int times = 5;
                int count = 4;
                auto algorithm = default_algorithm;
                bool include_self = false;
                cmd->add("--algorithm", &algorithm).set_help("The algorithm to use");
                cmd->set_true("-i", &include_self).set_help("also include the current cell when calculating the amount of nodes");
                cmd->add("--times", &times).set_help("How many to run");
                cmd->add("--count", &count).set_help("neighbour count");
                return cmd->on_complete([&]{
                    generator::add_spiky_rules(&rules, times, count, include_self, algorithm);
                    return argparse::ok;
                });
            });
            commands->add("clear", "clear smaller items", [&](argparse::SubParser* cmd)
            {
                cmd->parser_style = argparse::SubParserStyle::fallback;
                int times = 2;
                int count = 2;
                int range = 1;
                auto algorithm = default_algorithm;
                bool include_self = false;
                cmd->add("--algorithm", &algorithm).set_help("The algorithm to use");
                cmd->set_true("-i", &include_self).set_help("also include the current cell when calculating the amount of nodes");
                cmd->add("--times", &times).set_help("How many to run");
                cmd->add("--count", &count).set_help("neighbour count");
                cmd->add("--range", &range).set_help("the neighbour range");
                return cmd->on_complete([&]{
                    generator::add_clear_rules(&rules, times, count, range, include_self, algorithm);
                    return argparse::ok;
                });
            });
            commands->add("hblank", "add horizontal block", [&](argparse::SubParser* cmd)
            {
                cmd->parser_style = argparse::SubParserStyle::fallback;
                int y = 6;
                int height = 3;
                cmd->add("-y", &y).set_help("the y where to place it");
                cmd->add("--height", &height).set_help("the height of the block");
                return cmd->on_complete([&]{
                    generator::add_horizontal_blank_rule(&rules, y, height);
                    return argparse::ok;
                });
            });
            commands->add("smooth", "smooth map", [&](argparse::SubParser* cmd)
            {
                cmd->parser_style = argparse::SubParserStyle::fallback;
                int times = 5;
                int count = 4;
                auto algorithm = default_algorithm;
                bool include_self = false;
                cmd->add("--algorithm", &algorithm).set_help("The algorithm to use");
                cmd->set_true("-i", &include_self).set_help("also include the current cell when calculating the amount of nodes");
                cmd->add("--times", &times).set_help("How many to run");
                cmd->add("--count", &count).set_help("neighbour count");
                return cmd->on_complete([&]{
                    generator::add_simple_rules(&rules, times, count, include_self, algorithm);
                    return argparse::ok;
                });
            });
            commands->add("fill", "fill small holes", [&](argparse::SubParser* cmd)
            {
                cmd->parser_style = argparse::SubParserStyle::fallback;
                int hole_size = 3;
                bool allow_diagonals = true;
                cmd->add("--size", &hole_size).set_help("holes smaller than this are filled");
                cmd->set_false("-d", &allow_diagonals).set_help("include diagonals when flood filling");
                return cmd->on_complete([&]{
                    generator::add_fill_small_holes_rule(&rules, allow_diagonals, hole_size);
                    return argparse::ok;
                });
            });
            commands->add("only", "only keep a few holes", [&](argparse::SubParser* cmd)
            {
                cmd->parser_style = argparse::SubParserStyle::fallback;
                int keep = 1;
                bool allow_diagonals = true;
                cmd->add("--keep", &keep).set_help("the number of holes to keep");
                cmd->set_false("-d", &allow_diagonals).set_help("include diagonals when flood filling");
                return cmd->on_complete([&]{
                    generator::add_fill_all_holes_rule(&rules, allow_diagonals, keep);
                    return argparse::ok;
                });
            });

            return sub->on_complete
            (
                [&]
                {

                    auto world = generator::World::from_width_height(size.width, size.height);
                    world.clear(false);

                    if(rules.rules.empty())
                    {
                        std::cerr << "no rules specified";
                        return argparse::ok;
                    }

                    auto cell = generator::CellularAutomata{&rules, &world, Lrud{OutsideRule::wall}};

                    auto writer = CellWriter{debug, output, &world, world_scale};
                    writer.first_state();

                    while(cell.has_more_work())
                    {
                        cell.work();
                        writer.draw_step();
                    }

                    writer.done();

                    return argparse::ok;
                }
            );
        }
    );

    return argparse::parse_from_main(&parser, argc, argv);
}

