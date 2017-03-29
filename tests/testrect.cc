#include "gtest/gtest.h"
#include "core/rect.h"

#define GTEST(X) TEST(rect, X)

// constructor tests

GTEST(constructor_topleftwidthheight) {
  auto r = Rect<int>::FromTopLeftWidthHeight(2, 1, 3, 4);

  ASSERT_EQ(1, r.left);
  ASSERT_EQ(4, r.right); // left: 1, width_: 3
  ASSERT_EQ(2, r.top);
  ASSERT_EQ(6, r.bottom); // top: 2, height: 4

  ASSERT_FALSE(r.IsEmpty());
}

GTEST(constructor_widthheight) {
  auto r = Rect<int>::FromWidthHeight(3, 4);

  ASSERT_EQ(0, r.left);
  ASSERT_EQ(3, r.right);
  ASSERT_EQ(0, r.top);
  ASSERT_EQ(4, r.bottom);

  ASSERT_FALSE(r.IsEmpty());
}

GTEST(constructor_leftrighttopbottom) {
  auto r = Rect<int>::FromLeftRightTopBottom(1, 2, 3, 4);

  ASSERT_EQ(1, r.left);
  ASSERT_EQ(2, r.right);
  ASSERT_EQ(3, r.top);
  ASSERT_EQ(4, r.bottom);

  ASSERT_FALSE(r.IsEmpty());
}

// operation tests

GTEST(contains_point_exclusive) {
  const auto r = Rect<int>::FromWidthHeight(5, 5);

  // inside
  ASSERT_TRUE(r.ContainsExclusive(2, 2));

  // outside negative
  ASSERT_FALSE(r.ContainsExclusive(-2, 2));
  ASSERT_FALSE(r.ContainsExclusive(2, -2));
  ASSERT_FALSE(r.ContainsExclusive(-2, -2));

  // outside positive
  ASSERT_FALSE(r.ContainsExclusive(7, 2));
  ASSERT_FALSE(r.ContainsExclusive(2, 7));
  ASSERT_FALSE(r.ContainsExclusive(7, 7));

  // on the border
  ASSERT_FALSE(r.ContainsExclusive(0, 2));
  ASSERT_FALSE(r.ContainsExclusive(2, 0));
  ASSERT_FALSE(r.ContainsExclusive(0, 0));
  ASSERT_FALSE(r.ContainsExclusive(2, 5));
  ASSERT_FALSE(r.ContainsExclusive(5, 2));
  ASSERT_FALSE(r.ContainsExclusive(5, 5));
}

GTEST(contains_point_inclusive) {
  const auto r = Rect<int>::FromWidthHeight(5, 5);

  // inside
  ASSERT_TRUE(r.ContainsInclusive(2, 2));

  // outside negative
  ASSERT_FALSE(r.ContainsInclusive(-2, 2));
  ASSERT_FALSE(r.ContainsInclusive(2, -2));
  ASSERT_FALSE(r.ContainsInclusive(-2, -2));

  // outside positive
  ASSERT_FALSE(r.ContainsInclusive(7, 2));
  ASSERT_FALSE(r.ContainsInclusive(2, 7));
  ASSERT_FALSE(r.ContainsInclusive(7, 7));

  // on the border
  ASSERT_TRUE(r.ContainsInclusive(0, 2));
  ASSERT_TRUE(r.ContainsInclusive(2, 0));
  ASSERT_TRUE(r.ContainsInclusive(0, 0));
  ASSERT_TRUE(r.ContainsInclusive(2, 5));
  ASSERT_TRUE(r.ContainsInclusive(5, 2));
  ASSERT_TRUE(r.ContainsInclusive(5, 5));
}


GTEST(contains_rect_exclusive) {
  const auto r = Rect<int>::FromWidthHeight(10, 10);

  // inside
  ASSERT_TRUE(r.ContainsExclusive( Rect<int>::FromWidthHeight(2, 2).OffsetToCopy(2, 2) ));

  // outside negative
  ASSERT_FALSE(r.ContainsExclusive( Rect<int>::FromWidthHeight(2, 2).OffsetToCopy(-3, 3) ));
  ASSERT_FALSE(r.ContainsExclusive( Rect<int>::FromWidthHeight(2, 2).OffsetToCopy(3, -3) ));
  ASSERT_FALSE(r.ContainsExclusive( Rect<int>::FromWidthHeight(2, 2).OffsetToCopy(-3, -3) ));

  // outside positive
  ASSERT_FALSE(r.ContainsExclusive( Rect<int>::FromWidthHeight(2, 2).OffsetToCopy(15, 3) ));
  ASSERT_FALSE(r.ContainsExclusive( Rect<int>::FromWidthHeight(2, 2).OffsetToCopy(3, 15) ));
  ASSERT_FALSE(r.ContainsExclusive( Rect<int>::FromWidthHeight(2, 2).OffsetToCopy(15, 15) ));

  // over border
  ASSERT_FALSE(r.ContainsExclusive( Rect<int>::FromWidthHeight(15, 5).OffsetToCopy(-2, 2) ));
  ASSERT_FALSE(r.ContainsExclusive( Rect<int>::FromWidthHeight(5, 15).OffsetToCopy(2, -2) ));

  // all are outside
  ASSERT_FALSE(r.ContainsExclusive( Rect<int>::FromWidthHeight(15, 15).OffsetToCopy(-2, -2) ));
}


GTEST(insert) {
  const auto r = Rect<int>::FromWidthHeight(5, 10).InsetCopy(1, 2);

  ASSERT_EQ(1, r.left);
  ASSERT_EQ(4, r.right);
  ASSERT_EQ(2, r.top);
  ASSERT_EQ(8, r.bottom);
}

GTEST(offset) {
  const auto r = Rect<int>::FromWidthHeight(5, 10).OffsetCopy(1, 2);

  ASSERT_EQ(1, r.left);
  ASSERT_EQ(6, r.right);
  ASSERT_EQ(2, r.top);
  ASSERT_EQ(12, r.bottom);
}

GTEST(offset_to) {
  const auto r = Rect<int>::FromTopLeftWidthHeight(10, 12, 5, 10).OffsetToCopy(1, 2);

  ASSERT_EQ(1, r.left);
  ASSERT_EQ(6, r.right);
  ASSERT_EQ(2, r.top);
  ASSERT_EQ(12, r.bottom);
}

GTEST(empty) {
  auto r = Rect<int>::FromWidthHeight(5, 10);

  ASSERT_FALSE(r.IsEmpty());

  r.SetEmpty();

  ASSERT_TRUE(r.IsEmpty());
}

GTEST(default_empty) {
  Rect<int> r;
  ASSERT_TRUE(r.IsEmpty());
}

GTEST(sizes) {
  const auto r = Rect<int>::FromWidthHeight(5, 10);
  ASSERT_EQ(5, r.GetWidth());
  ASSERT_EQ(10, r.GetHeight());
}

GTEST(center) {
  const auto r = Rect<int>::FromTopLeftWidthHeight(3, 2, 6, 10);
  ASSERT_EQ(3, r.GetRelativeCenterX());
  ASSERT_EQ(5, r.GetRelativeCenterY());

  ASSERT_EQ(5, r.GetAbsoluteCenterX());
  ASSERT_EQ(8, r.GetAbsoluteCenterY());
}

