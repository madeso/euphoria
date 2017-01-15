#include "gtest/gtest.h"
#include "core/interpolate.h"
#include "tests/testbase.h"

#define GTEST(X) TEST(interpolate, X)

GTEST(do_nothing) {
  FloatInterpolate f {5.0f};
  f.Update(0.1f);

  EXPECT_NEAR(f.GetValue(), 5.0f, 0.0001f);
}

GTEST(basic) {
  FloatInterpolate f {0.0f};
  f.Linear(1.0f, 1.0f);
  f.Update(0.1f);

  EXPECT_NEAR(f.GetValue(), 0.1f, 0.0001f);
}

GTEST(basic_with_clear) {
  FloatInterpolate f {0.0f};
  f.Clear().Linear(1.0f, 1.0f);
  f.Update(0.1f);

  EXPECT_NEAR(f.GetValue(), 0.1f, 0.0001f);
}

GTEST(huge_step) {
  FloatInterpolate f {0.0f};
  f.Linear(1.0f, 1.0f);
  f.Update(5.0f);

  EXPECT_NEAR(f.GetValue(), 1.0f, 0.0001f);
}

GTEST(clear_after_a_time) {
  FloatInterpolate f {0.0f};
  f.Linear(1.0f, 1.0f);
  f.Update(0.5f);
  f.Clear();

  EXPECT_NEAR(f.GetValue(), 0.5f, 0.0001f);
}

GTEST(change_after_some_time) {
  FloatInterpolate f {0.0f};
  f.Linear(2.0f, 1.0f); // go to 2 this time
  f.Update(0.5f); // go half
  EXPECT_NEAR(f.GetValue(), 1.0f, 0.0001f);
  f.Clear().Linear(0.0f, 1.0f); // go back to 0
  f.Update(0.5f);
  EXPECT_NEAR(f.GetValue(), 0.5f, 0.0001f);
}

