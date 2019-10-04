#include "core/interpolate.h"
#include "tests/approx_equal.h"

#include "catch.hpp"

namespace euco = euphoria::core;

TEST_CASE("interpolate-do_nothing", "[interpolate]")
{
  euco::FloatInterpolate f{5.0f};
  f.Update(0.1f);

  REQUIRE(f.GetValue() == Approx(5.0f));
}

TEST_CASE("interpolate-basic", "[interpolate]")
{
  euco::FloatInterpolate f{0.0f};
  f.Linear(1.0f, 1.0f);
  f.Update(0.1f);

  REQUIRE(f.GetValue() == Approx(0.1f));
}

TEST_CASE("interpolate-basic_negative", "[interpolate]")
{
  euco::FloatInterpolate f{1.0f};
  f.Linear(0.0f, 1.0f);
  f.Update(0.1f);

  REQUIRE(f.GetValue() == Approx(0.9f));
}

TEST_CASE("interpolate-basic_with_clear", "[interpolate]")
{
  euco::FloatInterpolate f{0.0f};
  f.Clear().Linear(1.0f, 1.0f);
  f.Update(0.1f);

  REQUIRE(f.GetValue() == Approx(0.1f));
}

TEST_CASE("interpolate-huge_step", "[interpolate]")
{
  euco::FloatInterpolate f{0.0f};
  f.Linear(1.0f, 1.0f);
  f.Update(5.0f);

  REQUIRE_FALSE(f.HasSteps());
  REQUIRE(f.GetValue() == Approx(1.0f));
}

TEST_CASE("interpolate-clear_after_a_time", "[interpolate]")
{
  euco::FloatInterpolate f{0.0f};
  f.Linear(1.0f, 1.0f);
  f.Update(0.5f);
  REQUIRE(f.HasSteps());
  f.Clear();

  REQUIRE_FALSE(f.HasSteps());
  REQUIRE(f.GetValue() == Approx(0.5f));
}

TEST_CASE("interpolate-change_after_some_time", "[interpolate]")
{
  euco::FloatInterpolate f{0.0f};
  f.Linear(2.0f, 1.0f);  // go to 2 this time
  f.Update(0.5f);        // go half
  REQUIRE(f.GetValue() == Approx(1.0f));
  f.Clear().Linear(0.0f, 1.0f);  // go back to 0
  REQUIRE(f.GetValue() == Approx(1.0f));
  REQUIRE(f.Debug_GetFrom() == Approx(1.0f));
  f.Update(0.5f);
  REQUIRE(f.HasSteps());
  REQUIRE(f.GetValue() == Approx(0.5f));
}
