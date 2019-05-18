#include "core/generator_maze.h"

#include "core/random.h"
#include "core/draw.h"
#include "core/colors.h"

namespace generator
{
  vec2i DirToOffset(const Dir d)
  {
    switch(d)
    {
      case Dir::South: return vec2i{0, -1};
      case Dir::North: return vec2i{0,  1};
      case Dir::West : return vec2i{-1, 0};
      case Dir::East : return vec2i{ 1, 0};
      default: return vec2i(0,0);
    }
  }


  const std::vector<Dir>& AllDirs()
  {
    const static auto dirs = std::vector<Dir>{Dir::North, Dir::South, Dir::East, Dir::West};
    return dirs;
  }


  Dir FlipDirection(const Dir d)
  {
    switch(d)
    {
      case Dir::North: return Dir::South;
      case Dir::South: return Dir::North;
      case Dir::East:  return Dir::West;
      case Dir::West:  return Dir::East;
      default: return Dir::North;
    }
  }


  Cell::Type DirToCellPath(const Dir d)
  {
    switch(d)
    {
      case Dir::North: return Cell::PathNorth;
      case Dir::South: return Cell::PathSouth;
      case Dir::East:  return Cell::PathEast;
      case Dir::West:  return Cell::PathWest;
      default: return Cell::PathNorth;
    }
  }


  void Visit(Maze* maze, const point2i& np)
  {
    maze->RefValue(np.x, np.y) |= Cell::Visited;
  }


  point2i AddStepToMaze(Maze* maze, const point2i& c, Dir dir)
  {
    const auto o = DirToOffset(dir);
    const auto np = c + o;
    maze->RefValue(np.x, np.y) |= Cell::Visited | DirToCellPath(FlipDirection(dir));
    maze->RefValue(c.x, c.y) |= DirToCellPath(dir);
    return np;
  }


  bool HasVisited(Maze* maze, const point2i& np)
  {
    return (maze->Value(np.x, np.y) & Cell::Visited) != 0;
  }


  bool CanVisitWithoutMakingLoop(Maze* maze, const point2i& np)
  {
    const auto world_size = Recti::FromWidthHeight(maze->Width()-1, maze->Height() - 1);
    return world_size.ContainsInclusive(np) && !HasVisited(maze, np);
  }
  

  template<typename T>
  T PopRandom(std::vector<T>* vec, Random* r)
  {
    ASSERT(!vec->empty());
    const auto i = r->NextRange(vec->size());
    T t = (*vec)[i];
    vec->erase(vec->begin()+i);
    return t;
  }


  point2i RandomPositionOnMaze(Random* random, Maze* maze)
  {
    return {
      random->NextRange(maze->Width()),
      random->NextRange(maze->Height())
    };
  }


  //////////////////////////////////////////////////////////////////////////////////////////

  void RecursiveBacktracker::Setup()
  {
    maze->Clear(Cell::None);

    const auto random_position = RandomPositionOnMaze(random, maze);

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
    const auto c = stack.top();

    std::vector<Dir> neighbours;

    for(auto d: AllDirs())
    {
      const auto np = c + DirToOffset(d);
      if(CanVisitWithoutMakingLoop(maze, np))
      {
        neighbours.push_back(d);
      }
    }

    if(neighbours.empty())
    {
      stack.pop();
    }
    else
    {
      const Dir dir = random->Next(neighbours);
      auto np = AddStepToMaze(maze, c, dir);
      stack.push(np);
      visited_cells += 1;
    }
  }


  //////////////////////////////////////////////////////////////////////////////////////////

  void AddToFrontier(Maze* maze, std::vector<RandomTraversal::Entry>* frontier, const point2i& p)
  {
    Visit(maze, p);
    for(auto d: AllDirs())
    {
      if(CanVisitWithoutMakingLoop(maze, p + DirToOffset(d)))
      {
        frontier->push_back({p, d});
      }
    }
  }


  void RandomTraversal::Setup()
  {
    maze->Clear(Cell::None);

    AddToFrontier(maze, &frontier, RandomPositionOnMaze(random, maze));
  }


  bool RandomTraversal::HasMoreWork() const
  {
    return !frontier.empty();
  }


  void RandomTraversal::Work()
  {
    auto f = PopRandom(&frontier, random);
    const auto np = f.position + DirToOffset(f.direction);

    if(!CanVisitWithoutMakingLoop(maze, np)) { return; }
    AddStepToMaze(maze, f.position, f.direction);

    AddToFrontier(maze, &frontier, np);
  }


  //////////////////////////////////////////////////////////////////////////////////////////

  Drawer::Drawer()
    : wall_color( Color::Black )
    , cell_color( Color::LightGray )
    , cell_visited_color( Color::White )
    , unit_color ( Color::Red )
    , corridor_color ( Color::White )
  {}

  Rgbi Drawer::CalculateCellColor(int x, int y) const
  {
    const auto cell_value = maze->Value(x,y);

    if(tracker && tracker->HasMoreWork() && !tracker->stack.empty())
    {
      const auto t = tracker->stack.top();
      if(x == t.x && y == t.y)
      {
        return unit_color;
      }
    }

    if(traversal)
    {
      for(auto e: traversal->frontier)
      {
        auto p = e.position + DirToOffset(e.direction);
        if(p.x == x && p.y == y)
        {
          return unit_color;
        }
      }
    }

    if( cell_value & Cell::Visited )
    {
      return cell_visited_color;
    }
    else
    {
      return cell_color;
    }
  }

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
        const auto px = wall_size + x * path_size;
        const auto py = wall_size + y * path_size + cell_size - 1;

        draw.Square(CalculateCellColor(x, y), px, py, cell_size);

        const auto xywh = [](int x, int y, int w, int h)
        {
          return Recti::FromTopLeftWidthHeight(y+1,x,w,h);
        };

        const auto cell_value = maze->Value(x,y);

        if(cell_value & Cell::PathSouth)
        {
          draw.Rect(corridor_color, xywh(px, py-cell_size, cell_size, wall_size));
        }
        if(cell_value & Cell::PathEast)
        {
          draw.Rect(corridor_color, xywh(px+cell_size, py, wall_size, cell_size));
        }
      }
    }
  }
}

BEGIN_ENUM_LIST(generator::Dir)
  ENUM_VALUE(generator::Dir, North)
  ENUM_VALUE(generator::Dir, South)
  ENUM_VALUE(generator::Dir, East)
  ENUM_VALUE(generator::Dir, West)
END_ENUM_LIST()

