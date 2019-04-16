#include "core/generator_maze.h"
#include "core/random.h"
#include "core/imageops.h"

#include "core/debug.h"

#include <iostream>

// later when doing floodfill
// #include "core/colorbrewer.h"

void main()
{
  auto random = Random {};
  auto maze = generator::Maze::FromWidthHeight(20, 20);

  auto gen   = generator::RecursiveBacktracker{};
  gen.maze = &maze;
  gen.random = &random;
  gen.Setup();

  auto drawer = generator::Drawer {};
  drawer.maze = &maze;
  drawer.tracker = &gen;

  drawer.Draw();
  debug::MemoryChunkToFile(drawer.image.Write(ImageWriteFormat::PNG), "maze.png");

  while(gen.HasMoreWork())
  {
    gen.Work();

    drawer.Draw();
    debug::MemoryChunkToFile(drawer.image.Write(ImageWriteFormat::PNG), "maze.png");
  }

  /*
  auto table = ImageToStringTable(drawer.image,
    {
      {'#', drawer.wall_color},
      {'/', drawer.cell_color},
      {' ', drawer.cell_visited_color},
      {'O', drawer.unit_color}
    }
  );

  for(int r=0; r<table.Height(); r+=1)
  {
    for(int c=0; c<table.Height(); c+=1)
    {
      std::cout << table.Value(c, r);
    }
    std::cout << "\n";
  }
  */
}
