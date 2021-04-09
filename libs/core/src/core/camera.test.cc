#include "core/camera.h"

#include "catch.hpp"
#include "tests/approx.h"

namespace euco = euphoria::core;

using namespace euphoria::tests;

TEST_CASE("camera-clip2world", "[camera]")
{
    euco::Camera camera;

    const std::vector<float> aspects {1.0f, 2.0f, 0.5f};

    for(float aspect: aspects)
    {
        const auto cc = camera.Compile(aspect);
        CAPTURE(aspect);

        const std::vector<float> values {0.0f, 1.0f, 2.0f, -1.0f};

        for(float x: values)
        {
            for(float y: values)
            {
                for(float z: values)
                {
                    const auto start = euco::vec3f {x, y, z};
                    const auto world = cc.ClipToWorld(start);
                    const auto clip  = cc.WorldToClip(world);
                    REQUIRE(approx(start) == clip);
                }
            }
        }
    }
}

// not sure why this fails with infinte/divided by w=zero values
/*
TEST_CASE("camera-world2clip", "[camera]")
{
  const std::vector<float> aspects{1.0f, 2.0f, 0.5f};
  const std::vector<float> angles{0.0f, 45.0f, -90.f};
  const std::vector<float> values{0.0f, 1.0f, 2.0f, -1.0f};

  for(float aspect : aspects)
  {
    CAPTURE(aspect);

    for(float angle : angles)
    {
      for(float cx : values)
      {
        for(float cy : values)
        {
          for(float cz : values)
          {
            Camera camera;
            camera.position = vec3f{cx, cy, cz};
            camera.rotation = quatf::FromAxisAngle(AxisAngle::RightHandAround(
                vec3f::YAxis(), Angle::FromDegrees(angle)));
            const auto cc = camera.Compile(aspect);

            CAPTURE(camera.position);
            CAPTURE(angle);

            for(float x : values)
            {
              for(float y : values)
              {
                for(float z : values)
                {
                  const vec3f start = vec3f{x, y, z};
                  const vec3f clip  = cc.WorldToClip(start);
                  CAPTURE(clip);
                  const vec3f world = cc.ClipToWorld(clip);
                  CHECK(approx(start) == world);
                }
              }
            }
          }
        }
      }
    }
  }
}
*/
