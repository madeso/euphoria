#include "core/generator_maze.h"
#include "core/generator_cell.h"

#include "core/random.h"
#include "core/imageops.h"

#include "core/debug.h"

#include <iostream>

// later when doing floodfill
// #include "core/colorbrewer.h"

void maze()
{
  auto random = Random {};
  auto maze = generator::Maze::FromWidthHeight(30, 10);

  auto gen   = generator::RecursiveBacktracker{};
  gen.maze = &maze;
  gen.random = &random;
  gen.Setup();

  auto drawer = generator::Drawer {};
  drawer.maze = &maze;
  drawer.tracker = &gen;
  drawer.cell_size = 1;
  drawer.wall_size = 1;

  while(gen.HasMoreWork())
  {
    gen.Work();
  }

  drawer.Draw();
  debug::MemoryChunkToFile(drawer.image.Write(ImageWriteFormat::PNG), "maze.png");

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

void cell()
{
  auto random = Random {};
  auto world = generator::World::FromWidthHeight(80, 80);

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
  debug::MemoryChunkToFile(drawer.image.Write(ImageWriteFormat::PNG), "maze.png");
}

int main()
{
  // maze();
  cell();

  return 0;
}
