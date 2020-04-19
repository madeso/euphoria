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
    auto output = argparse::FileOutput {f};
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
            gen.reset(g.release());
        }
        break;
    case MazeAlgorithm::RandomTraversal:
        {
            auto g = std::make_unique<generator::RandomTraversal>();
            g->maze = &maze;
            g->random = &random;
            drawer.traversal = g.get();
            gen.reset(g.release());
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
                drawer.image.Write(ImageWriteFormat::PNG),
                output.NextFile()
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



struct Cellwriter
{
    bool debug;
    argparse::FileOutput output;
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

    Image
    GenerateWorldImage(const generator::World& w)
    {
        return Draw(w, Color::Black, Color::White, world_scale);
    }

    void
    FirstState()
    {
        if(!output.single)
        {
            auto img = GenerateWorldImage(*world);
            io::ChunkToFile(img.Write(ImageWriteFormat::PNG), output.NextFile());
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
            io::ChunkToFile(img.Write(ImageWriteFormat::PNG), output.file);
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
                img.Write(ImageWriteFormat::PNG),
                output.NextFile()
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
                    img.Write(ImageWriteFormat::PNG),
                    output.NextFile()
                );
            }
            write_index += 1;
        }

        for (int i = 0; i < 5; i += 1)
        {
            const auto img = GenerateWorldImage(world_copy);
            io::ChunkToFile
            (
                img.Write(ImageWriteFormat::PNG),
                output.NextFile()
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
    Add(argparse::ParserBase* base)
    {
        base->Add("--size", &size).Help("set the size");
        base->Add("-o, --output", &output).Help("specify output");

        base->Add("--cell", &cell_size).Help("set the cell size");
        base->Add("--wall", &wall_size).Help("set the wall size");
        base->SetTrue("-c,--console", &console).Help("foce console");
    }
};


constexpr NeighborhoodAlgorithm DEFAULT_ALGORITHM = NeighborhoodAlgorithm::Box;

int
main(int argc, char* argv[])
{
    auto parser = argparse::Parser{"Generate worlds"};
    auto sub = parser.AddSubParsers();

    sub->Add
    (
        "recursive",
        "maze generation using recursive backtracker algorithm",
        [&](argparse::SubParser* sub)
        {
            auto args = MazeArguments{};
            args.Add(sub);

            return sub->OnComplete
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
                    return argparse::ParseResult::Ok;
                }
            );
        }
    );
    
    sub->Add
    (
        "random",
        "maze generation using random traversal algorithm",
        [&](argparse::SubParser* sub)
        {
            auto args = MazeArguments{};
            args.Add(sub);

            return sub->OnComplete
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
                    return argparse::ParseResult::Ok;
                }
            );
        }
    );

    sub->Add
    (
        "cell",
        "world generation using cellular automata algorithm",
        [&](argparse::SubParser* sub)
        {
            int world_scale = 5;
            bool debug = false;
            Sizei size = Sizei::FromWidthHeight(100, 70);
            std::string output = "cell.png";
            auto random = Random {};
            auto rules = generator::Rules{};
            
            sub->Add("--size", &size).Help("set the size");
            sub->Add("-o, --output", &output).Help("specify output");
            sub->Add("--scale", &world_scale).Help("set the scale");
            sub->SetTrue("--debug", &debug);

            auto commands = sub->AddSubParsers("commands");

            commands->Add("random", "random fill", [&](argparse::SubParser* cmd)
            {
                cmd->parser_style = argparse::SubParserStyle::Fallback;
                Fourway<BorderSetupRule> border_control = Fourway{BorderSetupRule::AlwaysWall};
                float random_fill = 0.5;
                cmd->Add("--fill", &random_fill).Help("How much to fill");
                cmd->Add("-bc", &border_control)
                    .Help("Change how the border is generated")
                    ;
                return cmd->OnComplete([&]{
                    generator::AddRandomFill(&rules, &random, random_fill, border_control);
                    return argparse::ParseResult::Ok;
                });
            });
            commands->Add("combo", "smooth map with combo rule", [&](argparse::SubParser* cmd)
            {
                cmd->parser_style = argparse::SubParserStyle::Fallback;
                int times = 4;
                int count = 5;
                int small = 2;
                auto algorithm = DEFAULT_ALGORITHM;
                bool include_self = false;
                cmd->Add("--algorithm", &algorithm).Help("The algorithm to use");
                cmd->SetTrue("-i", &include_self).Help("also include the current cell when calculating the amount of nodes");
                cmd->Add("--times", &times).Help("How many to run");
                cmd->Add("--count", &count).Help("neighbour count");
                cmd->Add("--small", &small).Help("small rule");
                return cmd->OnComplete([&]{
                    generator::AddComboRules(&rules, times, count, small, include_self, algorithm);
                    return argparse::ParseResult::Ok;
                });
            });
            commands->Add("spiky", "smooth map with spikes", [&](argparse::SubParser* cmd)
            {
                cmd->parser_style = argparse::SubParserStyle::Fallback;
                int times = 5;
                int count = 4;
                auto algorithm = DEFAULT_ALGORITHM;
                bool include_self = false;
                cmd->Add("--algorithm", &algorithm).Help("The algorithm to use");
                cmd->SetTrue("-i", &include_self).Help("also include the current cell when calculating the amount of nodes");
                cmd->Add("--times", &times).Help("How many to run");
                cmd->Add("--count", &count).Help("neighbour count");
                return cmd->OnComplete([&]{
                    generator::AddSpikyRules(&rules, times, count, include_self, algorithm);
                    return argparse::ParseResult::Ok;
                });
            });
            commands->Add("clear", "clear smaller items", [&](argparse::SubParser* cmd)
            {
                cmd->parser_style = argparse::SubParserStyle::Fallback;
                int times = 2;
                int count = 2;
                int range = 1;
                auto algorithm = DEFAULT_ALGORITHM;
                bool include_self = false;
                cmd->Add("--algorithm", &algorithm).Help("The algorithm to use");
                cmd->SetTrue("-i", &include_self).Help("also include the current cell when calculating the amount of nodes");
                cmd->Add("--times", &times).Help("How many to run");
                cmd->Add("--count", &count).Help("neighbour count");
                cmd->Add("--range", &range).Help("the neighbour range");
                return cmd->OnComplete([&]{
                    generator::AddClearRules(&rules, times, count, range, include_self, algorithm);
                    return argparse::ParseResult::Ok;
                });
            });
            commands->Add("hblank", "add horizontal block", [&](argparse::SubParser* cmd)
            {
                cmd->parser_style = argparse::SubParserStyle::Fallback;
                int y = 6;
                int height = 3;
                cmd->Add("-y", &y).Help("the y where to place it");
                cmd->Add("--height", &height).Help("the height of the block");
                return cmd->OnComplete([&]{
                    generator::AddHorizontalBlankRule(&rules, y, height);
                    return argparse::ParseResult::Ok;
                });
            });
            commands->Add("smooth", "smooth map", [&](argparse::SubParser* cmd)
            {
                cmd->parser_style = argparse::SubParserStyle::Fallback;
                int times = 5;
                int count = 4;
                auto algorithm = DEFAULT_ALGORITHM;
                bool include_self = false;
                cmd->Add("--algorithm", &algorithm).Help("The algorithm to use");
                cmd->SetTrue("-i", &include_self).Help("also include the current cell when calculating the amount of nodes");
                cmd->Add("--times", &times).Help("How many to run");
                cmd->Add("--count", &count).Help("neighbour count");
                return cmd->OnComplete([&]{
                    generator::AddSimpleRules(&rules, times, count, include_self, algorithm);
                    return argparse::ParseResult::Ok;
                });
            });
            commands->Add("fill", "fill small holes", [&](argparse::SubParser* cmd)
            {
                cmd->parser_style = argparse::SubParserStyle::Fallback;
                int size = 3;
                bool allow_diagonals = true;
                cmd->Add("--size", &size).Help("holes smaller than this are filled");
                cmd->SetFalse("-d", &allow_diagonals).Help("include diagonals when flood filling");
                return cmd->OnComplete([&]{
                    generator::AddFillSmallHolesRule(&rules, allow_diagonals, size);
                    return argparse::ParseResult::Ok;
                });
            });

            return sub->OnComplete
            (
                [&]
                {

                    auto world = generator::World::FromWidthHeight(size.width, size.height);
                    world.Clear(false);

                    if(rules.rules.empty())
                    {
                        std::cerr << "no rules specified";
                        return argparse::ParseResult::Ok;
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

                    return argparse::ParseResult::Ok;
                }
            );
        }
    );

    return argparse::ParseFromMain(&parser, argc, argv);
}

