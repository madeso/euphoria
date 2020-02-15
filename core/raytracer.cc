#include "core/raytracer.h"

#include "core/image.h"

namespace euphoria::core
{
    void
    Raytrace(Image* aimage)
    {
      Image& img = *aimage;

      for(int y=0; y<img.GetHeight(); y+=1)
      for(int x=0; x<img.GetWidth(); x+=1)
      {
          auto color = Rgb
          {
              x / static_cast<float>(img.GetWidth()),
              y / static_cast<float>(img.GetHeight()),
              0.0f
          };
          img.SetPixel(x,y, rgbi(color));
      }
    }
}

