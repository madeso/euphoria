#include "core/generator_cell.h"

#include "core/random.h"
#include "core/draw.h"
#include "core/colors.h"

namespace generator
{
  void CellularAutomata::Setup()
  {
    world->SetAll([this](unsigned x, unsigned y) {
        if( border_control != BorderControl::Random )
        {
          const auto is_border = x==0 || x == world->Width()-1 || y==0 || y==world->Height()-1;
          return border_control == BorderControl::AlwaysEmpty || is_border;
        }
        return random->NextFloat01() < random_fill;
      }
    );
  }

  bool CellularAutomata::HasMoreWork() const
  {
    return iteration < 5;
  }

  int CountWalls(const World& world, int cx, int cy, int step=1)
  {
    int walls = 0;
    for(int y=cy-step; y<=cy+step; y+=1)
    {
      for(int x=cx-step; x<=cx+step; x+=1)
      {
        if(x==cx && y==cy) continue;
        if(!world.Indices().ContainsInclusive(x,y))
        {
          // todo: make this an option
          walls += 1;
        }
        else
        {
          if(world.Value(x,y)) walls += 1;
        }
      }
    }
    return walls;
  }

  void SmoothMap(World* world)
  {
    const World current = *world;
    world->SetAll([&current](int x, int y) {
        const auto walls = CountWalls(current, x, y);
        if( walls > 4 ) { return true; }
        if( walls < 4 ) { return false; }
        return current.Value(x,y);
      }
    );
  }

  void CellularAutomata::Work()
  {
    SmoothMap(world);
    iteration += 1;
  }

  void Draw(const World& world, Image* image, Rgbi wall_color, Rgbi space_color)
  {
    // todo: implement me!  
  }
}

