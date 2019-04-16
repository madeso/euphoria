#include "core/argparse.h"

#include "core/generator_maze.h"
#include "core/generator_cell.h"

#include "core/random.h"
#include "core/imageops.h"

#include "core/stringutils.h"
#include "core/debug.h"

#include <iostream>
#include <sstream>
#include <iomanip>

// later when doing floodfill
// #include "core/colorbrewer.h"

struct Output
{
  explicit Output(const std::string& o)
    : file(o)
    , single(!EndsWith(o, "/"))
  {
  }

  std::string NextFile()
  {
    std::ostringstream ss;
    index += 1;
    std::cout << "Generating " << index << "...\n";
    ss << file << std::setfill('0') << std::setw(5) << index << ".png";
    return ss.str();
  }

  std::string file;
  bool single;
  int index = 0;
};

void maze(int world_width, int world_height, int cell_size, int wall_size, const std::string& f, bool console)
{
  auto output = Output{f};
  auto random = Random {};
  auto maze = generator::Maze::FromWidthHeight(world_width, world_height);

  auto gen   = generator::RecursiveBacktracker{};
  gen.maze = &maze;
  gen.random = &random;
  gen.Setup();

  auto drawer = generator::Drawer {};
  drawer.maze = &maze;
  drawer.tracker = &gen;
  drawer.cell_size = cell_size;
  drawer.wall_size = wall_size;

  if(!output.single)
  {
    drawer.Draw();
    debug::MemoryChunkToFile(drawer.image.Write(ImageWriteFormat::PNG), output.NextFile());
  }

  while(gen.HasMoreWork())
  {
    gen.Work();

    if(!output.single)
    {
      drawer.Draw();
      debug::MemoryChunkToFile(drawer.image.Write(ImageWriteFormat::PNG), output.NextFile());
    }
  }

  drawer.Draw();

  if(!console && output.single)
  {
    debug::MemoryChunkToFile(drawer.image.Write(ImageWriteFormat::PNG), output.file);
  }

  if(!output.single)
  {
    drawer.Draw();
    for(int i=0; i<5; i+=1)
    {
      debug::MemoryChunkToFile(drawer.image.Write(ImageWriteFormat::PNG), output.NextFile());
    }
  }

  if(console)
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
}

void cell(int world_width, int world_height, const std::string& output)
{
  auto random = Random {};
  auto world = generator::World::FromWidthHeight(world_width, world_height);

  generator::CellularAutomata cell;
  cell.world = &world;
  cell.random = &random;
  cell.Setup();

  auto drawer = generator::CellularAutomataDrawer {};
  drawer.world = &world;

  while(cell.HasMoreWork())
  {
    cell.Work();
  }

  drawer.Draw();
  debug::MemoryChunkToFile(drawer.image.Write(ImageWriteFormat::PNG), output);
}

int main(int argc, char* argv[])
{
  auto parser = argparse::Parser{"Generate worlds"};

  int world_width = 10;
  int world_height = 10;
  std::string output = "maze.png";
  bool console = false;

  int cell_size = 1;
  int wall_size = 1;

  parser.AddSimple("--cell", cell_size).Help("set the cell size");
  parser.AddSimple("--wall", wall_size).Help("set the wall size");
  parser.AddSimple("--width", world_width).Help("set the height");
  parser.AddSimple("--height", world_height).Help("set the width");
  parser.AddSimple("-o", output).Help("specify output");
  parser.AddSimple("-c", console).Help("foce console");

  auto maze_command = [&]() {
    maze(world_width, world_height, cell_size, wall_size, output, console);
  };
  auto cell_command = [&]() {
    cell(world_width, world_height, output);
  };

  // todo: fix this setup
  // parser.AddSimpleFunction("-maze", maze_command);
  // parser.AddSimpleFunction("-cell", cell_command);

  std::string command;
  parser.AddSimple("command", command).Help("specify either cell or maze");

  auto status = parser.Parse(argc, argv);

  if(!status.error.empty())
  {
    std::cerr << status.error;
  }
  std::cout << status.out;

  if(command == "maze")
  {
    maze_command();
  }
  else if (command == "cell")
  {
    cell_command();
  }

  return 0;
}
