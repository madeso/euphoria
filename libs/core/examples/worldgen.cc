#include "core/argparse.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include "core/generator_maze.h"
#include "core/generator_cell.h"
#include "core/random.h"
#include "core/imageops.h"
#include "core/stringutils.h"
#include "core/io.h"
#include "core/knuthshuffle.h"

// later when doing floodfill
// #include "core/colorbrewer.h"


using namespace euphoria::core;


struct Difference
{
    int x;
    int y;
    bool new_value;
};


std::vector<Difference>
FindDifferences(const Table<bool>& src, const Table<bool>& dst)
{
    std::vector<Difference> ret;
    for(int y = 0; y < src.GetHeight(); y += 1)
    {
        for(int x = 0; x < src.GetWidth(); x += 1)
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
PrintMazeToConsole(const generator::Drawer& drawer)
{
    const auto table = ImageToStringTable
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

    const auto strings = ToStrings(table);
    for(const auto& s: strings)
    {
        std::cout << s << "\n";
    }
}


enum class MazeAlgorithm
{
    RecursiveBacktracker,
    RandomTraversal
};


void
HandleMazeCommand
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
    auto output = argparse::file_output {f};
    auto random = Random{};
    auto maze = generator::Maze::FromWidthHeight(world_width, world_height);

    auto drawer = generator::Drawer {};

    std::unique_ptr<generator::Algorithm> gen;

    switch(algo)
    {
    case MazeAlgorithm::RecursiveBacktracker:
        {
            auto g = std::make_unique<generator::RecursiveBacktracker>();
            g->maze = &maze;
            g->random = &random;
            drawer.tracker = g.get();
            gen = std::move(g);
        }
        break;
    case MazeAlgorithm::RandomTraversal:
        {
            auto g = std::make_unique<generator::RandomTraversal>();
            g->maze = &maze;
            g->random = &random;
            drawer.traversal = g.get();
            gen = std::move(g);
        }
        break;
    default:
        DIE("Unhandled");
    }

    gen->Setup();

    drawer.maze = &maze;
    drawer.cell_size = cell_size;
    drawer.wall_size = wall_size;

    auto draw_frame = [&]()
    {
        if(!output.single)
        {
            drawer.Draw();
            io::ChunkToFile
            (
                drawer.image.write(image_write_format::png),
                output.get_next_file()
            );
        }
    };

    draw_frame();

    while(gen->HasMoreWork())
    {
        gen->Work();
        draw_frame();
    }

    drawer.Draw();

    if(console)
    {
        PrintMazeToConsole(drawer);
    }
    else
    {
        if(output.single)
        {
            io::ChunkToFile
            (
                drawer.image.write(image_write_format::png),
                output.file
            );
        }
        else
        {
            for(int i = 0; i < 5; i += 1)
            {
                io::ChunkToFile
                (
                    drawer.image.write(image_write_format::png),
                    output.get_next_file()
                );
            }
        }
    }
}



struct Cellwriter
{
    bool debug;
    argparse::file_output output;
    generator::World* world;
    int world_scale;

    Random shuffle_random;
    generator::World world_copy;

    explicit Cellwriter
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

    [[nodiscard]] image
    GenerateWorldImage(const generator::World& world_or_copy) const
    {
        return Draw
        (
            world_or_copy,
            {Color::Black},
            {Color::White},
            world_scale,
            std::nullopt
        );
    }

    void
    FirstState()
    {
        if(!output.single)
        {
            auto img = GenerateWorldImage(*world);
            io::ChunkToFile(img.write(image_write_format::png), output.get_next_file());
        }

        world_copy = *world;
        DrawStep();
    }

    void
    Done()
    {
        if(output.single)
        {
            auto img = GenerateWorldImage(*world);
            io::ChunkToFile(img.write(image_write_format::png), output.file);
        }
        else
        {
            DrawStep();
        }
    }

    void
    DrawStep()
    {
        if(output.single) { return; }
        if (debug)
        {
            const auto img = GenerateWorldImage(*world);
            io::ChunkToFile
            (
                img.write(image_write_format::png),
                output.get_next_file()
            );
        }
        else
        {
            ShuffleDraw();
        }
    }

    void
    ShuffleDraw()
    {
        auto diffs = FindDifferences(*world, world_copy);
        KnuthShuffle(&diffs, &shuffle_random);
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
                const auto img = GenerateWorldImage(world_copy);
                io::ChunkToFile
                (
                    img.write(image_write_format::png),
                    output.get_next_file()
                );
            }
            write_index += 1;
        }

        for (int i = 0; i < 5; i += 1)
        {
            const auto img = GenerateWorldImage(world_copy);
            io::ChunkToFile
            (
                img.write(image_write_format::png),
                output.get_next_file()
            );
        }
    }
};


struct MazeArguments
{
    Sizei size = Sizei::FromWidthHeight(10, 10);
    std::string output = "maze.png";

    int cell_size = 1;
    int wall_size = 1;
    bool console = false;

    void
    Add(argparse::parser_base* base)
    {
        base->add("--size", &size).set_help("set the size");
        base->add("-o, --output", &output).set_help("specify output");

        base->add("--cell", &cell_size).set_help("set the cell size");
        base->add("--wall", &wall_size).set_help("set the wall size");
        base->set_true("-c,--console", &console).set_help("foce console");
    }
};


constexpr NeighborhoodAlgorithm DEFAULT_ALGORITHM = NeighborhoodAlgorithm::Box;

int
main(int argc, char* argv[])
{
    auto parser = argparse::parser{"Generate worlds"};
    auto sub = parser.add_sub_parsers();

    sub->add
    (
        "recursive",
        "maze generation using recursive backtracker algorithm",
        [&](argparse::sub_parser* sub)
        {
            auto args = MazeArguments{};
            args.Add(sub);

            return sub->on_complete
            (
                [&]
                {
                    HandleMazeCommand
                    (
                        MazeAlgorithm::RecursiveBacktracker,
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

    sub->add
    (
        "random",
        "maze generation using random traversal algorithm",
        [&](argparse::sub_parser* sub)
        {
            auto args = MazeArguments{};
            args.Add(sub);

            return sub->on_complete
            (
                [&]
                {
                    HandleMazeCommand
                    (
                        MazeAlgorithm::RandomTraversal,
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

    sub->add
    (
        "cell",
        "world generation using cellular automata algorithm",
        [&](argparse::sub_parser* sub)
        {
            int world_scale = 5;
            bool debug = false;
            Sizei size = Sizei::FromWidthHeight(100, 70);
            std::string output = "cell.png";
            auto random = Random {};
            auto rules = generator::Rules{};

            sub->add("--size", &size).set_help("set the size");
            sub->add("-o, --output", &output).set_help("specify output");
            sub->add("--scale", &world_scale).set_help("set the scale");
            sub->set_true("--debug", &debug);

            auto commands = sub->add_sub_parsers("commands");

            commands->add("random", "random fill", [&](argparse::sub_parser* cmd)
            {
                cmd->parser_style = argparse::sub_parser_style::fallback;
                Fourway<BorderSetupRule> border_control = Fourway{BorderSetupRule::AlwaysWall};
                float random_fill = 0.5;
                cmd->add("--fill", &random_fill).set_help("How much to fill");
                cmd->add("-bc", &border_control)
                    .set_help("Change how the border is generated")
                    ;
                return cmd->on_complete([&]{
                    generator::AddRandomFill(&rules, &random, random_fill, border_control);
                    return argparse::ok;
                });
            });
            commands->add("combo", "smooth map with combo rule", [&](argparse::sub_parser* cmd)
            {
                cmd->parser_style = argparse::sub_parser_style::fallback;
                int times = 4;
                int count = 5;
                int small = 2;
                auto algorithm = DEFAULT_ALGORITHM;
                bool include_self = false;
                cmd->add("--algorithm", &algorithm).set_help("The algorithm to use");
                cmd->set_true("-i", &include_self).set_help("also include the current cell when calculating the amount of nodes");
                cmd->add("--times", &times).set_help("How many to run");
                cmd->add("--count", &count).set_help("neighbour count");
                cmd->add("--small", &small).set_help("small rule");
                return cmd->on_complete([&]{
                    generator::AddComboRules(&rules, times, count, small, include_self, algorithm);
                    return argparse::ok;
                });
            });
            commands->add("spiky", "smooth map with spikes", [&](argparse::sub_parser* cmd)
            {
                cmd->parser_style = argparse::sub_parser_style::fallback;
                int times = 5;
                int count = 4;
                auto algorithm = DEFAULT_ALGORITHM;
                bool include_self = false;
                cmd->add("--algorithm", &algorithm).set_help("The algorithm to use");
                cmd->set_true("-i", &include_self).set_help("also include the current cell when calculating the amount of nodes");
                cmd->add("--times", &times).set_help("How many to run");
                cmd->add("--count", &count).set_help("neighbour count");
                return cmd->on_complete([&]{
                    generator::AddSpikyRules(&rules, times, count, include_self, algorithm);
                    return argparse::ok;
                });
            });
            commands->add("clear", "clear smaller items", [&](argparse::sub_parser* cmd)
            {
                cmd->parser_style = argparse::sub_parser_style::fallback;
                int times = 2;
                int count = 2;
                int range = 1;
                auto algorithm = DEFAULT_ALGORITHM;
                bool include_self = false;
                cmd->add("--algorithm", &algorithm).set_help("The algorithm to use");
                cmd->set_true("-i", &include_self).set_help("also include the current cell when calculating the amount of nodes");
                cmd->add("--times", &times).set_help("How many to run");
                cmd->add("--count", &count).set_help("neighbour count");
                cmd->add("--range", &range).set_help("the neighbour range");
                return cmd->on_complete([&]{
                    generator::AddClearRules(&rules, times, count, range, include_self, algorithm);
                    return argparse::ok;
                });
            });
            commands->add("hblank", "add horizontal block", [&](argparse::sub_parser* cmd)
            {
                cmd->parser_style = argparse::sub_parser_style::fallback;
                int y = 6;
                int height = 3;
                cmd->add("-y", &y).set_help("the y where to place it");
                cmd->add("--height", &height).set_help("the height of the block");
                return cmd->on_complete([&]{
                    generator::AddHorizontalBlankRule(&rules, y, height);
                    return argparse::ok;
                });
            });
            commands->add("smooth", "smooth map", [&](argparse::sub_parser* cmd)
            {
                cmd->parser_style = argparse::sub_parser_style::fallback;
                int times = 5;
                int count = 4;
                auto algorithm = DEFAULT_ALGORITHM;
                bool include_self = false;
                cmd->add("--algorithm", &algorithm).set_help("The algorithm to use");
                cmd->set_true("-i", &include_self).set_help("also include the current cell when calculating the amount of nodes");
                cmd->add("--times", &times).set_help("How many to run");
                cmd->add("--count", &count).set_help("neighbour count");
                return cmd->on_complete([&]{
                    generator::AddSimpleRules(&rules, times, count, include_self, algorithm);
                    return argparse::ok;
                });
            });
            commands->add("fill", "fill small holes", [&](argparse::sub_parser* cmd)
            {
                cmd->parser_style = argparse::sub_parser_style::fallback;
                int size = 3;
                bool allow_diagonals = true;
                cmd->add("--size", &size).set_help("holes smaller than this are filled");
                cmd->set_false("-d", &allow_diagonals).set_help("include diagonals when flood filling");
                return cmd->on_complete([&]{
                    generator::AddFillSmallHolesRule(&rules, allow_diagonals, size);
                    return argparse::ok;
                });
            });
            commands->add("only", "only keep a few holes", [&](argparse::sub_parser* cmd)
            {
                cmd->parser_style = argparse::sub_parser_style::fallback;
                int keep = 1;
                bool allow_diagonals = true;
                cmd->add("--keep", &keep).set_help("the number of holes to keep");
                cmd->set_false("-d", &allow_diagonals).set_help("include diagonals when flood filling");
                return cmd->on_complete([&]{
                    generator::AddFillAllHolesRule(&rules, allow_diagonals, keep);
                    return argparse::ok;
                });
            });

            return sub->on_complete
            (
                [&]
                {

                    auto world = generator::World::FromWidthHeight(size.width, size.height);
                    world.Clear(false);

                    if(rules.rules.empty())
                    {
                        std::cerr << "no rules specified";
                        return argparse::ok;
                    }

                    auto cell = generator::CellularAutomata{&rules, &world, Fourway{OutsideRule::Wall}};

                    auto writer = Cellwriter{debug, output, &world, world_scale};
                    writer.FirstState();

                    while(cell.HasMoreWork())
                    {
                        cell.Work();
                        writer.DrawStep();
                    }

                    writer.Done();

                    return argparse::ok;
                }
            );
        }
    );

    return argparse::parse_from_main(&parser, argc, argv);
}

