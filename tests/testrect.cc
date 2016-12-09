#include "gtest/gtest.h"
#include "spacetyper/rect.h"

#define GTEST(X) TEST(rect, X)

GTEST(basic) {
  auto r = Rect<int>::FromLeftTopWidthHeight(1, 2, 3, 4);

  ASSERT_EQ(1, r.left);
  ASSERT_EQ(2, r.top);
  ASSERT_EQ(6, r.bottom); // top: 2, height: 4
  ASSERT_EQ(4, r.right); // left: 1, width: 3
}
