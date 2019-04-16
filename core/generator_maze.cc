#include "core/generator_maze.h"

#include "core/random.h"
#include "core/draw.h"
#include "core/colors.h"

namespace generator
{
  void RecursiveBacktracker::Setup()
  {
    maze->Clear(Cell::None);

    const auto random_position = point2i {
      random->NextRange(maze->Width()),
      random->NextRange(maze->Height())
    };
    stack.push(random_position);
    maze->Value(random_position.x, random_position.y, Cell::Visited);
    visited_cells = 1;
  }

  bool RecursiveBacktracker::HasMoreWork() const
  {
    return visited_cells < maze->Width() * maze->Height();
  }

  void RecursiveBacktracker::Work()
  {
    auto d2o = [](const Dir d)
    {
      switch(d)
      {
        case Dir::South: return vec2i{0, -1};
        case Dir::North: return vec2i{0,  1};
        case Dir::West : return vec2i{-1, 0};
        case Dir::East : return vec2i{ 1, 0};
        default: return vec2i(0,0);
      }
    };
    const auto c = stack.top();
    std::vector<Dir> neighbours;
    const auto world_size = Recti::FromWidthHeight(maze->Width()-1, maze->Height() - 1);
    auto add_neighbour = [&](Dir d) {
      const auto np = c + d2o(d);
      if( world_size.ContainsInclusive(np)
          && (maze->Value(np.x, np.y) & Cell::Visited) == 0 )
      {
        neighbours.push_back(d);
      }
    };
    add_neighbour(Dir::North);
    add_neighbour(Dir::South);
    add_neighbour(Dir::East);
    add_neighbour(Dir::West);

    if(neighbours.empty())
    {
      stack.pop();
    }
    else
    {
      const Dir dir = random->Next(neighbours);
      const auto flipdir = [](const Dir d) -> Dir {
        switch(d)
        {
          case Dir::North: return Dir::South;
          case Dir::South: return Dir::North;
          case Dir::East:  return Dir::West;
          case Dir::West:  return Dir::East;
          default: return Dir::North;
        }
      };
      const auto dir2path = [](const Dir d) -> Cell::Type {
        switch(d)
        {
          case Dir::North: return Cell::PathNorth;
          case Dir::South: return Cell::PathSouth;
          case Dir::East:  return Cell::PathEast;
          case Dir::West:  return Cell::PathWest;
          default: return Cell::PathNorth;
        }
      };
      const auto o = d2o(dir);
      const auto np = c + o;
      maze->RefValue(np.x, np.y) |= Cell::Visited | dir2path(flipdir(dir));
      maze->RefValue(c.x, c.y) |= dir2path(dir);
      stack.push(np);
      visited_cells += 1;
    }
  }

  Drawer::Drawer()
    : wall_color( Color::Black )
    , cell_color( Color::Gray )
    , cell_visited_color( Color::White )
    , unit_color ( Color::Red )
  {}

  void Drawer::Draw()
  {
    const auto path_size = cell_size + wall_size;
    image.SetupNoAlphaSupport(
        wall_size + maze->Width()*path_size,
        wall_size + maze->Height() * path_size);
    auto draw = ::Draw{&image};

    draw.Clear(wall_color);

    for(unsigned int x=0; x<maze->Width(); x+=1)
    {
      for(unsigned int y=0; y<maze->Height(); y+=1)
      {
        const auto xywh = [](int x, int y, int w, int h)
        {
          return Recti::FromTopLeftWidthHeight(x,y,w,h);
        };
        const auto cell_value = maze->Value(x,y);
        const auto px = wall_size + x * path_size;
        const auto py = wall_size + y * path_size;
        Rgbi color = cell_color;
        if( cell_value & Cell::Visited )
        {
          color = cell_visited_color;
        }
        if(tracker && tracker->HasMoreWork() && !tracker->stack.empty())
        {
          const auto t = tracker->stack.top();
          if(x == t.x && y == t.y)
          {
            color = unit_color;
          }
        }

        draw.Square(color,
            xywh(px, py, cell_size, cell_size)
            );


        if(cell_value & Cell::PathSouth)
        {
          draw.Square(cell_color, xywh(px+cell_size, py-cell_size, cell_size, wall_size));
        }
        if(cell_value & Cell::PathEast)
        {
          draw.Square(cell_color, xywh(px+cell_size, py, wall_size, cell_size));
        }
      }
    }
  }
}

