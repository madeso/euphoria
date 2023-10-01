#include "core/intersection.h"
#include "tests/approx_equal.h"
#include "catch.hpp"
#include "base/ray.h"

using namespace eu;
using namespace eu::core;


#if 0

#include "base/random.h"
#include "fmt/core.h"

vec3f random_on_sphere(Random* r)
{
  const auto r1 = r->get_next_float01();
  const auto r2 = r->get_next_float01();
  const auto lat = acos(2*r1 - 1) - Angle::from_percent_of_360(0.25f);
  const auto lon = Angle::from_percent_of_360(r2);

  return
  {
    cos(lat) * cos(lon),
    cos(lat) * sin(lon),
    sin(lat)
  };
}

bool print_test(Random* r, int max_tests)
{
    const float scale = 100.0f;

    static int ok_tests = 0;
    static int fa_tests = 0;

    const auto add_ok = ok_tests < max_tests;
    const auto add_fa = fa_tests < max_tests;

    if(!add_ok && !add_fa) { return false; }

    const auto orig = random_on_sphere(r) * scale;
    const auto dir = random_on_sphere(r).as_normalized();

    const auto v0 = random_on_sphere(r) * scale;
    const auto v1 = random_on_sphere(r) * scale;
    const auto v2 = random_on_sphere(r) * scale;

    const auto result = get_intersection_ray_triangle
    (
        UnitRay3f(orig, dir),
        v0, v1, v2
    );

    const auto print_test = [&](const char* name, int i)
    {
        fmt::print("SECTION(\"{}_{}\")\n{{\n", name, i);
        fmt::print("    const auto result = get_intersection_ray_triangle\n");
        fmt::print("    (\n");
        fmt::print("        UnitRay3f\n");
        fmt::print("        (\n");
        fmt::print("            vec3f{{{}f, {}f, {}f}},\n", orig.x, orig.y, orig.z);
        fmt::print("            vec3f{{{}f, {}f, {}f}}.as_normalized()\n", dir.x, dir.y, dir.z);
        fmt::print("        ),\n");
        fmt::print("        vec3f{{{}f, {}f, {}f}},\n", v0.x, v0.y, v0.z);
        fmt::print("        vec3f{{{}f, {}f, {}f}},\n", v1.x, v1.y, v1.z);
        fmt::print("        vec3f{{{}f, {}f, {}f}}\n", v2.x, v2.y, v2.z);
        fmt::print("    );\n");
    };

    if(result)
    {
        if(!add_ok) { return true; }
        ok_tests += 1;
        
        print_test("collision", ok_tests);
        
        fmt::print("    REQUIRE(result);\n");
        fmt::print("    CHECK(*result == Approx({}));\n", *result);
        
        fmt::print("}}\n");
    }
    else
    {
        if(!add_fa) { return true; }
        fa_tests += 1;

        print_test("miss", fa_tests);
        fmt::print("    CHECK_FALSE(result);\n");
        fmt::print("}}\n");
    }

    return true;
}


void generate_random_tests(int max_tests)
{
    Random rand;
    while(print_test(&rand, max_tests))
    {
        // nop
    }
}
#endif


TEST_CASE("intersection-ray/triangle", "[intersection]")
{
    SECTION("miss_1")
    {
        const auto result = get_intersection_ray_triangle
        (
            UnitRay3f
            (
                vec3f{-51.16545f, 76.01922f, -40.03966f},
                vec3f{-0.11030686f, 0.4981688f, 0.860035f}.as_normalized()
            ),
            vec3f{-79.75484f, 46.01236f, -39.013187f},
            vec3f{-33.85421f, 83.0767f, 44.1832f},
            vec3f{-27.321648f, -95.348206f, 12.737671f}
        );
        CHECK_FALSE(result);
    }
    SECTION("miss_2")
    {
        const auto result = get_intersection_ray_triangle
        (
            UnitRay3f
            (
                vec3f{66.39531f, 72.726425f, 17.393341f},
                vec3f{0.915316f, 0.09530963f, -0.39129618f}.as_normalized()
            ),
            vec3f{-2.2524679f, 85.1597f, -52.371292f},
            vec3f{62.699192f, -21.869518f, 74.76988f},
            vec3f{-27.610731f, 49.575672f, -82.34015f}
        );
        CHECK_FALSE(result);
    }
    SECTION("miss_3")
    {
        const auto result = get_intersection_ray_triangle
        (
            UnitRay3f
            (
                vec3f{85.443054f, 46.802326f, -22.561613f},
                vec3f{0.51887566f, 0.5492123f, 0.6550831f}.as_normalized()
            ),
            vec3f{-47.92611f, 66.538704f, 57.23364f},
            vec3f{-24.22051f, -55.37177f, 79.67016f},
            vec3f{34.15698f, 56.770634f, -74.90257f}
        );
        CHECK_FALSE(result);
    }
    SECTION("miss_4")
    {
        const auto result = get_intersection_ray_triangle
        (
            UnitRay3f
            (
                vec3f{-6.6768355f, 88.043755f, 46.943768f},
                vec3f{-0.70160854f, 0.18344218f, -0.6885451f}.as_normalized()
            ),
            vec3f{-29.903337f, -40.43928f, -86.43179f},
            vec3f{-38.589195f, 88.618095f, 25.645805f},
            vec3f{49.30065f, -67.14384f, 55.32768f}
        );
        CHECK_FALSE(result);
    }
    SECTION("miss_5")
    {
        const auto result = get_intersection_ray_triangle
        (
            UnitRay3f
            (
                vec3f{68.6031f, -12.875435f, 71.60892f},
                vec3f{0.19107912f, -0.6492108f, -0.73621607f}.as_normalized()
            ),
            vec3f{-30.31062f, 46.607456f, 83.12047f},
            vec3f{58.31067f, 81.04437f, -5.6281137f},
            vec3f{37.19748f, -92.79547f, -2.3125136f}
        );
        CHECK_FALSE(result);
    }

    SECTION("collision_1")
    {
        const auto result = get_intersection_ray_triangle
        (
            UnitRay3f
            (
                vec3f{-33.40818f, -84.66033f, 41.430927f},
                vec3f{0.7495933f, 0.2794702f, -0.6000052f}.as_normalized()
            ),
            vec3f{-95.25679f, 28.816341f, -9.785809f},
            vec3f{30.02708f, -35.32267f, -88.60408f},
            vec3f{47.333054f, -84.06681f, 26.312601f}
        );
        REQUIRE(result);
        CHECK(*result == Approx(85.52471));
    }
    SECTION("collision_2")
    {
        const auto result = get_intersection_ray_triangle
        (
            UnitRay3f
            (
                vec3f{-22.257559f, -83.49188f, -50.33593f},
                vec3f{-0.03254954f, 0.953412f, 0.29991025f}.as_normalized()
            ),
            vec3f{-56.528694f, -46.0924f, -68.41051f},
            vec3f{-50.820602f, -62.413406f, 59.34503f},
            vec3f{73.32132f, -65.6623f, 17.67616f}
        );
        REQUIRE(result);
        CHECK(*result == Approx(33.192245));
    }
    SECTION("collision_3")
    {
        const auto result = get_intersection_ray_triangle
        (
            UnitRay3f
            (
                vec3f{-50.53874f, 27.621418f, -81.74896f},
                vec3f{-0.05385356f, -0.9737135f, 0.22131832f}.as_normalized()
            ),
            vec3f{6.306717f, 60.709488f, -79.212265f},
            vec3f{-78.31093f, 0.72770774f, -62.184155f},
            vec3f{-27.764107f, -53.843563f, -79.561455f}
        );
        REQUIRE(result);
        CHECK(*result == Approx(51.045776));
    }
    SECTION("collision_4")
    {
        const auto result = get_intersection_ray_triangle
        (
            UnitRay3f
            (
                vec3f{-65.6977f, -39.771793f, -64.047f},
                vec3f{0.6284047f, 0.68981254f, 0.35953617f}.as_normalized()
            ),
            vec3f{-77.84602f, -11.690011f, -61.671238f},
            vec3f{61.165726f, -34.963314f, -70.96703f},
            vec3f{-11.927585f, -98.50488f, -12.430616f}
        );
        REQUIRE(result);
        CHECK(*result == Approx(20.742744));
    }
    SECTION("collision_5")
    {
        const auto result = get_intersection_ray_triangle
        (
            UnitRay3f
            (
                vec3f{40.17801f, 81.88964f, -40.985542f},
                vec3f{-0.8009491f, -0.58379394f, 0.13291061f}.as_normalized()
            ),
            vec3f{-63.861404f, 34.219685f, 68.92557f},
            vec3f{57.09964f, -58.577393f, -57.517998f},
            vec3f{-97.64034f, -9.205268f, -19.535276f}
        );
        REQUIRE(result);
        CHECK(*result == Approx(159.43541));
    }
}
