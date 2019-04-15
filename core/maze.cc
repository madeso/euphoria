#include "core/maze.h"

#include "core/random.h"
#include "core/draw.h"
#include "core/colors.h"

namespace generator
{
  void RecursiveBacktracker::Setup()
  {
    world->Clear(Cell::None);

    const auto random_position = point2i {
      random->NextRange(world->Width()),
      random->NextRange(world->Height())
    };
    stack.push(random_position);
    world->Value(random_position.x, random_position.y, Cell::Visited);
    visited_cells = 1;
  }

  bool RecursiveBacktracker::HasMoreWork() const
  {
    return visited_cells < world->Width() * world->Height();
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
    const auto world_size = Recti::FromWidthHeight(world->Width()-1, world->Height() - 1);
    auto add_neighbour = [&](Dir d) {
      const auto np = c + d2o(d);
      if( world_size.ContainsInclusive(np)
          && (world->Value(np.x, np.y) & Cell::Visited) == 0 )
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
      world->RefValue(np.x, np.y) |= Cell::Visited | dir2path(flipdir(dir));
      world->RefValue(c.x, c.y) |= dir2path(dir);
      stack.push(np);
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
        wall_size + world->Width()*path_size,
        wall_size + world->Height() * path_size);
    auto draw = ::Draw{&image};

    draw.Clear(wall_color);

    const auto size = path_size;
    for(unsigned int x=0; x<world->Width(); x+=1)
    {
      for(unsigned int y=0; y<world->Height(); y+=1)
      {
        const auto xywh = [](int x, int y, int w, int h)
        {
          return Recti::FromTopLeftWidthHeight(x,y,w,h);
        };
        const auto s = wall_size;
        const auto c = cell_size;

        const auto cell_value = world->Value(x,y);
        const auto px = s+x*size;
        const auto py = s+y*size;
        Rgbi cl = cell_color;
        if( cell_value & Cell::Visited )
        {
          cl = cell_visited_color;
        }
        if(tracker && tracker->HasMoreWork() && !tracker->stack.empty())
        {
          const auto t = tracker->stack.top();
          if(x == t.x && y == t.y)
          {
            cl = unit_color;
          }
        }

        draw.Square(cl,
            xywh(px, py, c, c)
            );


        if(cell_value & Cell::PathSouth)
        {
          draw.Square(cell_color, xywh(px+c, py+c, c, s));
        }
        if(cell_value & Cell::PathEast)
        {
          draw.Square(cell_color, xywh(px+c, py+c, s, c));
        }
      }
    }
  }
}

