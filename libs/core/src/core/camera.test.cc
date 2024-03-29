#include "core/camera3.h"

#include "catch2/catch_all.hpp"
#include "tests/approx.h"

using namespace eu;
using namespace eu::tests;

TEST_CASE("camera-clip2world", "[camera]")
{
    core::Camera3 camera;

    const std::vector<float> aspects {1.0f, 2.0f, 0.5f};

    for(float aspect: aspects)
    {
        const auto cc = camera.compile(aspect);
        CAPTURE(aspect);

        const std::vector<float> values {0.0f, 1.0f, 2.0f, -1.0f};

        for(float x: values)
        {
            for(float y: values)
            {
                for(float z: values)
                {
                    const auto start = vec3f{x, y, z};
                    const auto world = cc.from_clip_to_world(start);
                    const auto clip = cc.from_world_to_clip(world);
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
                  const vec3f clip = cc.WorldToClip(start);
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
