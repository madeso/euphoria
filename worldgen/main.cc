#include "core/argparse.h"

#include "core/generator_maze.h"
#include "core/generator_cell.h"

#include "core/random.h"
#include "core/imageops.h"

#include "core/stringutils.h"
#include "core/debug.h"
#include "core/randomvector.h"

#include <iostream>
#include <sstream>
#include <iomanip>

// later when doing floodfill
// #include "core/colorbrewer.h"

struct Difference
{
  int x; int y;
  bool new_value;
};

std::vector<Difference> FindDifferences(const Table<bool>& src, const Table<bool>& dst)
{
  std::vector<Difference> ret;
  for(int y=0; y<src.Height(); y+=1)
  {
    for(int x=0; x<src.Width(); x+=1)
    {
      const auto lhs = src.Value(x, y);
      const auto rhs = dst.Value(x, y);
      if(lhs != rhs)
      {
        ret.push_back({x, y, lhs});
      }
    }
  }
  return ret;
}

void PrintMazeToConsole(const generator::Drawer& drawer, const generator::Maze& maze)
{
  auto table = ImageToStringTable(drawer.image,
    {
      {'#', drawer.wall_color},
      {'/', drawer.cell_color},
      {' ', drawer.wall_color},
      {' ', drawer.cell_visited_color},
      {'O', drawer.unit_color}
    }
  );

  for(int r=0; r<table.Height(); r+=1)
  {
    for(int c=0; c<table.Width(); c+=1)
    {
      std::cout << table.Value(c, r);
    }
    std::cout << "\n";
  }
}

struct Output
{
  explicit Output(const std::string& o)
    : file(o)
    , single(!EndsWith(o, "/"))
  {
  }

  std::string NextFile(bool print=true)
  {
    std::ostringstream ss;
    index += 1;
    if(print)
    {
      std::cout << "Generating " << index << "...\n";
    }
    ss << file << std::setfill('0') << std::setw(5) << index << ".png";
    return ss.str();
  }

  std::string file;
  bool single;
  int index = 0;
};

enum class MazeAlgorithm
{
  RecursiveBacktracker, RandomTraversal
};

void maze(MazeAlgorithm algo, int world_width, int world_height, int cell_size, int wall_size, const std::string& f, bool console)
{
  auto output = Output{f};
  auto random = Random {};
  auto maze = generator::Maze::FromWidthHeight(world_width, world_height);

  auto drawer = generator::Drawer {};

  std::unique_ptr<generator::Algorithm> gen;

  switch(algo)
  {
    case MazeAlgorithm::RecursiveBacktracker:
      {
        auto g   = std::make_unique<generator::RecursiveBacktracker>();
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
      ASSERT(false && "Unhandled");
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
      debug::MemoryChunkToFile(drawer.image.Write(ImageWriteFormat::PNG), output.NextFile());
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
    PrintMazeToConsole(drawer, maze);
  }
  else
  {
    if(output.single)
    {
      debug::MemoryChunkToFile(drawer.image.Write(ImageWriteFormat::PNG), output.file);
    }
    else
    {
      for(int i=0; i<5; i+=1)
      {
        debug::MemoryChunkToFile(drawer.image.Write(ImageWriteFormat::PNG), output.NextFile());
      }
    }
  }
}

void cell(int world_width, int world_height, const std::string& f, int world_scale)
{
  auto output = Output{f};

  auto random = Random {};
  auto world = generator::World::FromWidthHeight(world_width, world_height);

  generator::CellularAutomata cell;
  cell.world = &world;
  cell.random = &random;

  auto drawer = generator::CellularAutomataDrawer {};
  drawer.world = &world;
  drawer.scale = world_scale;
  world.Clear(false);

  if(!output.single)
  {
    drawer.Draw();
    debug::MemoryChunkToFile(drawer.image.Write(ImageWriteFormat::PNG), output.NextFile());
  }

  auto world_copy = world;
  if(!output.single)
  {
    drawer.world = &world_copy;
  }
  auto shuffle_random = Random{};
  auto draw_multi = [&]()
  {
    if(!output.single)
    {
      auto diffs = FindDifferences(world, world_copy);
      Shuffle(&diffs, &shuffle_random);
      int dindex = 0;
      const auto s = (diffs.size() / 25);
      const auto m = s < 2 ? 2 : s;
      for(const auto d: diffs)
      {
        // std::cout << "Setting " << d.x << " " << d.y << " to " << d.new_value << "\n";
        world_copy.Value(d.x, d.y, d.new_value);
        if((dindex % m) == 0)
        {
          drawer.Draw();
          debug::MemoryChunkToFile(drawer.image.Write(ImageWriteFormat::PNG), output.NextFile());
        }
        dindex += 1;
      }
      for(int i=0; i<5; i+=1)
      {
        debug::MemoryChunkToFile(drawer.image.Write(ImageWriteFormat::PNG), output.NextFile());
      }
    }
  };
  
  cell.Setup();

  draw_multi();


  while(cell.HasMoreWork())
  {
    std::cout << "Work #######\n";
    cell.Work();
    draw_multi();
  }

  drawer.Draw();
  if(output.single)
  {
    debug::MemoryChunkToFile(drawer.image.Write(ImageWriteFormat::PNG), output.file);
  }
  else
  {
    draw_multi();
  }

}

int main(int argc, char* argv[])
{
  auto parser = argparse::Parser{"Generate worlds"};

  int world_width = 10;
  int world_height = 10;
  int world_scale = 1;
  std::string output = "maze.png";
  bool console = false;

  int cell_size = 1;
  int wall_size = 1;

  parser.AddSimple("--cell", &cell_size).Help("set the cell size");
  parser.AddSimple("--wall", &wall_size).Help("set the wall size");
  parser.AddSimple("--width", &world_width).Help("set the height");
  parser.AddSimple("--height", &world_height).Help("set the width");
  parser.AddSimple("--scale", &world_scale).Help("set the scale");
  parser.AddSimple("-o", &output).Help("specify output");
  parser.AddSimple("-c", &console).Help("foce console");

  auto maze_command = [&](MazeAlgorithm algo) {
    maze(algo, world_width, world_height, cell_size, wall_size, output, console);
  };
  auto cell_command = [&]() {
    cell(world_width, world_height, output, world_scale);
  };

  // todo: fix this setup
  // parser.AddSimpleFunction("-maze", maze_command);
  // parser.AddSimpleFunction("-cell", cell_command);

  std::string command;
  parser.AddSimple("command", &command).Help("specify either cell or maze");

  const auto status = parser.Parse(argc, argv);
  if(status != argparse::ParseResult::Ok)
  {
    return -1;
  }

  if(command == "recursive")
  {
    maze_command(MazeAlgorithm::RecursiveBacktracker);
  }
  else if(command == "random")
  {
    maze_command(MazeAlgorithm::RandomTraversal);
  }
  else if (command == "cell")
  {
    cell_command();
  }
  else
  {
    std::cerr << "Unknown command: " << command << "\n";
  }

  return 0;
}
