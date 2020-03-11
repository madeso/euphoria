#include "core/argparse.h"

#include "core/generator_maze.h"
#include "core/generator_cell.h"

#include "core/random.h"
#include "core/imageops.h"

#include "core/stringutils.h"
#include "core/io.h"
#include "core/knuthshuffle.h"

#include <iostream>
#include <sstream>
#include <iomanip>

// later when doing floodfill
// #include "core/colorbrewer.h"

using namespace euphoria::core;

struct Difference
{
    int  x;
    int  y;
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
    const auto table = ImageToStringTable(
            drawer.image,
            {{'#', drawer.wall_color},
             {'/', drawer.cell_color},
             {' ', drawer.wall_color},
             {' ', drawer.cell_visited_color},
             {'O', drawer.unit_color}});
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
    auto output = argparse::FileOutput {f};
    auto random = Random {};
    auto maze   = generator::Maze::FromWidthHeight(world_width, world_height);

    auto drawer = generator::Drawer {};

    std::unique_ptr<generator::Algorithm> gen;

    switch(algo)
    {
    case MazeAlgorithm::RecursiveBacktracker: {
        auto g         = std::make_unique<generator::RecursiveBacktracker>();
        g->maze        = &maze;
        g->random      = &random;
        drawer.tracker = g.get();
        gen.reset(g.release());
    }
    break;
    case MazeAlgorithm::RandomTraversal: {
        auto g           = std::make_unique<generator::RandomTraversal>();
        g->maze          = &maze;
        g->random        = &random;
        drawer.traversal = g.get();
        gen.reset(g.release());
    }
    break;
    default: DIE("Unhandled");
    }

    gen->Setup();

    drawer.maze      = &maze;
    drawer.cell_size = cell_size;
    drawer.wall_size = wall_size;

    auto draw_frame = [&]() {
        if(!output.single)
        {
            drawer.Draw();
            io::ChunkToFile(
                    drawer.image.Write(ImageWriteFormat::PNG),
                    output.NextFile());
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
                drawer.image.Write(ImageWriteFormat::PNG),
                output.file
            );
        }
        else
        {
            for(int i = 0; i < 5; i += 1)
            {
                io::ChunkToFile
                (
                    drawer.image.Write(ImageWriteFormat::PNG),
                    output.NextFile()
                );
            }
        }
    }
}


void
HandleCellCommand
(
    bool debug,
    float fill,
    int world_width,
    int world_height,
    Fourway<BorderSetupRule> bc,
    Fourway<OutsideRule> outside_rule,
    const std::string& f,
    int world_scale
)
{
    auto output = argparse::FileOutput {f};

    auto random = Random {};
    auto world  = generator::World::FromWidthHeight(world_width, world_height);

    generator::CellularAutomata cell;
    cell.world  = &world;
    cell.random = &random;

    generator::SetupSimpleRules(&cell);
    // generator::SetupBasicRules(&cell);

    cell.border_control = bc;
    cell.outside_rule = outside_rule;
    cell.random_fill = fill;

    auto draw_world = [&](const generator::World& world)
    {
        return Draw(world, Color::Black, Color::White, world_scale);
    };
    world.Clear(false);

    if(!output.single)
    {
        auto img = draw_world(world);
        io::ChunkToFile(img.Write(ImageWriteFormat::PNG), output.NextFile());
    }

    auto shuffle_random = Random {};
    auto world_copy = world;
    auto draw_single_step = [&]()
    {
        if(!output.single)
        {
            if (debug)
            {
                const auto img = draw_world(world);
                io::ChunkToFile
                (
                    img.Write(ImageWriteFormat::PNG),
                    output.NextFile()
                );
            }
            else
            {
                auto diffs = FindDifferences(world, world_copy);
                KnuthShuffle(&diffs, &shuffle_random);
                int dindex = 0;
                const auto s = (diffs.size() / 25);
                const auto m = s < 2 ? 2 : s;
                for (const auto d : diffs)
                {
                    world_copy(d.x, d.y) = d.new_value;
                    if ((dindex % m) == 0)
                    {
                        const auto img = draw_world(world_copy);
                        io::ChunkToFile
                        (
                            img.Write(ImageWriteFormat::PNG),
                            output.NextFile()
                        );
                    }
                    dindex += 1;
                }

                for (int i = 0; i < 5; i += 1)
                {
                    const auto img = draw_world(world_copy);
                    io::ChunkToFile
                    (
                        img.Write(ImageWriteFormat::PNG),
                        output.NextFile()
                    );
                }
            }
        }
    };

    cell.Setup();

    draw_single_step();

    while(cell.HasMoreWork())
    {
        cell.Work();
        draw_single_step();
    }

    if(output.single)
    {
        auto img = draw_world(world);
        io::ChunkToFile(img.Write(ImageWriteFormat::PNG), output.file);
    }
    else
    {
        draw_single_step();
    }
}


int
main(int argc, char* argv[])
{
    auto parser = argparse::Parser {"Generate worlds"};

    int world_width  = 10;
    int world_height = 10;
    std::string output = "maze.png";

    int world_scale = 1;
    BorderSetupRule border_control = BorderSetupRule::AlwaysWall;
    float random_fill = 0.5;
    bool debug = false;

    int cell_size = 1;
    int wall_size = 1;
    bool console = false;

    auto add_common = [&](argparse::Parser& parser)
    {
        parser.AddSimple("--width", &world_width).Help("set the height");
        parser.AddSimple("--height", &world_height).Help("set the width");
        parser.AddSimple("-o, --output", &output).Help("specify output");
    };

    auto add_maze = [&](argparse::Parser& parser)
    {
        parser.AddSimple("--cell", &cell_size).Help("set the cell size");
        parser.AddSimple("--wall", &wall_size).Help("set the wall size");
        parser.SetTrue("-c,--console", &console).Help("foce console");
    };

    auto add_cell = [&](argparse::Parser& parser) {
        parser.AddSimple("--scale", &world_scale).Help("set the scale");
        parser.AddSimple("--fill", &random_fill).Help("How much to fill");
        parser.AddEnum("-bc, --border_control", &border_control).Help("Change how the border is generated");
        parser.SetTrue("--debug", &debug);
    };

    auto maze_command = [&](MazeAlgorithm algo)
    {
        HandleMazeCommand
        (
            algo,
            world_width,
            world_height,
            cell_size,
            wall_size,
            output,
            console
        );
    };

    auto precursive = parser.AddSubParser
    (
        "recursive",
        "maze generation using recursive backtracker algorithm",
        [&]{ maze_command(MazeAlgorithm::RecursiveBacktracker); }
    );
    add_common(*precursive);
    add_maze(*precursive);

    auto prandom = parser.AddSubParser
    (
        "random",
        "maze generation using random traversal algorithm",
        [&] { maze_command(MazeAlgorithm::RandomTraversal); }
    );
    add_common(*prandom);
    add_maze(*prandom);

    auto pcell = parser.AddSubParser
    (
        "cell",
        "world generation using cellular automata algorithm",
        [&]
        {
            HandleCellCommand
            (
                debug,
                random_fill,
                world_width,
                world_height,
                Fourway{ border_control },
                Fourway{ OutsideRule::Wall },
                output,
                world_scale
            );
        }
    );
    add_common(*pcell);
    add_cell(*pcell);

    const auto status = parser.Parse(argc, argv);
    if(status != argparse::ParseResult::Ok)
    {
        return -1;
    }

    return 0;
}
