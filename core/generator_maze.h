#ifndef CORE_GENERATOR_MAZE_H
#define CORE_GENERATOR_MAZE_H

#include "core/table.h"
#include "core/vec2.h"
#include "core/image.h"

#include <stack>

class Random;

namespace generator
{
  namespace Cell
  {
    enum Type
    {
      None       = 0,
      PathNorth  = 1 << 0,
      PathSouth  = 1 << 1,
      PathEast   = 1 << 2,
      PathWest   = 1 << 3,
      Visited    = 1 << 4
    };
  }

  enum class Dir
  {
    North, South, East, West
  };

  using Maze = Table<int>;

  // todo: implement more generators
  // https://bost.ocks.org/mike/algorithms/#maze-generation
  // https://gamedev.stackexchange.com/questions/75632/procedural-generation-of-tile-based-2d-world
  // https://unity3d.com/learn/tutorials/s/procedural-cave-generation-tutorial
  // https://gamedev.stackexchange.com/questions/153734/generate-cave-like-terrain-in-2d
  // http://www.gamasutra.com/blogs/HermanTulleken/20161005/282629/Algorithms_for_making_more_interesting_mazes.php


  struct RecursiveBacktracker
  {
    Maze* maze = nullptr;
    Random* random = nullptr;

    std::stack<point2i> stack;
    int visited_cells = 0;

    void Setup();
    bool HasMoreWork() const;
    void Work();
  };

  struct Drawer
  {
    Maze* maze = nullptr;

    int cell_size = 3;
    int wall_size = 1;

    RecursiveBacktracker* tracker = nullptr;

    Rgbi wall_color;
    Rgbi cell_color;
    Rgbi cell_visited_color;
    Rgbi unit_color;

    Image image;

    Drawer();
    void Draw();
  };

}

#endif  // CORE_GENERATOR_MAZE_H
