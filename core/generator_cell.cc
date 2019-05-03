#include "core/generator_cell.h"

#include "core/random.h"
#include "core/draw.h"
#include "core/colors.h"
#include "core/image.h"

namespace generator
{
  void CellularAutomata::Setup()
  {
    world->SetAll([this](unsigned x, unsigned y) {
        if( border_control != BorderControl::RandomWall &&
            border_control != BorderControl::RandomEmpty )
        {
          const auto is_border = x==0
                              || y==0
                              || x == world->Width()-1
                              || y==world->Height()-1
                              ;
          if(is_border)
          {
            return border_control == BorderControl::AlwaysWall;
          }
        }
        return random->NextFloat01() < random_fill;
      }
    );
  }

  bool CellularAutomata::HasMoreWork() const
  {
    return iteration < 5;
  }

  int CountWalls(const World& world, bool outside_is_wall, int cx, int cy, int step=1)
  {
    int walls = 0;
    for(int y=cy-step; y<=cy+step; y+=1)
    {
      for(int x=cx-step; x<=cx+step; x+=1)
      {
        if(x==cx && y==cy) continue;
        if(!world.Indices().ContainsInclusive(x,y))
        {
          if(outside_is_wall)
          {
            walls += 1;
          }
        }
        else
        {
          if(world.Value(x,y)) walls += 1;
        }
      }
    }
    return walls;
  }

  void SmoothMap(World* world, bool outside_is_wall)
  {
    const World current = *world;
    world->SetAll([&current, outside_is_wall](int x, int y) {
        const auto walls = CountWalls(current, outside_is_wall, x, y);
        if( walls > 4 ) { return true; }
        if( walls < 4 ) { return false; }
        return current.Value(x,y);
      }
    );
  }

  void CellularAutomata::Work()
  {
    SmoothMap(world,
        border_control == BorderControl::AlwaysWall ||
        border_control == BorderControl::RandomWall );
    iteration += 1;
  }

  CellularAutomataDrawer::CellularAutomataDrawer()
    : wall_color( Color::Black )
    , space_color( Color::White )
  {}

  void CellularAutomataDrawer::Draw()
  {
    image.SetupNoAlphaSupport(
        world->Width() * scale,
        world->Height() * scale);
    
    auto draw = ::Draw{&image};
    draw.Clear(space_color);

    for(unsigned int x=0; x<world->Width(); x+=1)
    {
      for(unsigned int y=0; y<world->Height(); y+=1)
      {
        const auto px = x * scale;
        const auto py = y * scale;
        const auto color = world->Value(x,y) ? wall_color : space_color;
        draw.Square(color, px, py+scale-1, scale);
      }
    }
  }
}

