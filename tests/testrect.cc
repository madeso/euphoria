#include "core/rect.h"

#include "catch.hpp"

// constructor tests

TEST_CASE("rect-constructor_topleftwidthheight", "[rect]")
{
  auto r = Rect<int>::FromTopLeftWidthHeight(2, 1, 3, 4);

  REQUIRE(r.left == 1);
  REQUIRE(r.right == 4);  // left: 1, width_: 3
  REQUIRE(r.top == 2);
  REQUIRE(r.bottom == 6);  // top: 2, height: 4

  REQUIRE_FALSE(r.IsEmpty());
}

TEST_CASE("rect-constructor_widthheight", "[rect]")
{
  auto r = Rect<int>::FromWidthHeight(3, 4);

  REQUIRE(r.left == 0);
  REQUIRE(r.right == 3);
  REQUIRE(r.top == 0);
  REQUIRE(r.bottom == 4);

  REQUIRE_FALSE(r.IsEmpty());
}

TEST_CASE("rect-constructor_leftrighttopbottom", "[rect]")
{
  auto r = Rect<int>::FromLeftRightTopBottom(1, 2, 3, 4);

  REQUIRE(r.left == 1);
  REQUIRE(r.right == 2);
  REQUIRE(r.top == 3);
  REQUIRE(r.bottom == 4);

  REQUIRE_FALSE(r.IsEmpty());
}

// operation tests

TEST_CASE("rect-contains_point_exclusive", "[rect]")
{
  const auto r = Rect<int>::FromWidthHeight(5, 5);

  // inside
  REQUIRE(r.ContainsExclusive(2, 2));

  // outside negative
  REQUIRE_FALSE(r.ContainsExclusive(-2, 2));
  REQUIRE_FALSE(r.ContainsExclusive(2, -2));
  REQUIRE_FALSE(r.ContainsExclusive(-2, -2));

  // outside positive
  REQUIRE_FALSE(r.ContainsExclusive(7, 2));
  REQUIRE_FALSE(r.ContainsExclusive(2, 7));
  REQUIRE_FALSE(r.ContainsExclusive(7, 7));

  // on the border
  REQUIRE_FALSE(r.ContainsExclusive(0, 2));
  REQUIRE_FALSE(r.ContainsExclusive(2, 0));
  REQUIRE_FALSE(r.ContainsExclusive(0, 0));
  REQUIRE_FALSE(r.ContainsExclusive(2, 5));
  REQUIRE_FALSE(r.ContainsExclusive(5, 2));
  REQUIRE_FALSE(r.ContainsExclusive(5, 5));
}

TEST_CASE("rect-contains_point_inclusive", "[rect]")
{
  const auto r = Rect<int>::FromWidthHeight(5, 5);

  // inside
  REQUIRE(r.ContainsInclusive(2, 2));

  // outside negative
  REQUIRE_FALSE(r.ContainsInclusive(-2, 2));
  REQUIRE_FALSE(r.ContainsInclusive(2, -2));
  REQUIRE_FALSE(r.ContainsInclusive(-2, -2));

  // outside positive
  REQUIRE_FALSE(r.ContainsInclusive(7, 2));
  REQUIRE_FALSE(r.ContainsInclusive(2, 7));
  REQUIRE_FALSE(r.ContainsInclusive(7, 7));

  // on the border
  REQUIRE(r.ContainsInclusive(0, 2));
  REQUIRE(r.ContainsInclusive(2, 0));
  REQUIRE(r.ContainsInclusive(0, 0));
  REQUIRE(r.ContainsInclusive(2, 5));
  REQUIRE(r.ContainsInclusive(5, 2));
  REQUIRE(r.ContainsInclusive(5, 5));
}


TEST_CASE("rect-contains_rect_exclusive", "[rect]")
{
  const auto r = Rect<int>::FromWidthHeight(10, 10);

  // inside
  REQUIRE(
      r.ContainsExclusive(Rect<int>::FromWidthHeight(2, 2).OffsetToCopy(2, 2)));

  // outside negative
  REQUIRE_FALSE(r.ContainsExclusive(
      Rect<int>::FromWidthHeight(2, 2).OffsetToCopy(-3, 3)));
  REQUIRE_FALSE(r.ContainsExclusive(
      Rect<int>::FromWidthHeight(2, 2).OffsetToCopy(3, -3)));
  REQUIRE_FALSE(r.ContainsExclusive(
      Rect<int>::FromWidthHeight(2, 2).OffsetToCopy(-3, -3)));

  // outside positive
  REQUIRE_FALSE(r.ContainsExclusive(
      Rect<int>::FromWidthHeight(2, 2).OffsetToCopy(15, 3)));
  REQUIRE_FALSE(r.ContainsExclusive(
      Rect<int>::FromWidthHeight(2, 2).OffsetToCopy(3, 15)));
  REQUIRE_FALSE(r.ContainsExclusive(
      Rect<int>::FromWidthHeight(2, 2).OffsetToCopy(15, 15)));

  // over border
  REQUIRE_FALSE(r.ContainsExclusive(
      Rect<int>::FromWidthHeight(15, 5).OffsetToCopy(-2, 2)));
  REQUIRE_FALSE(r.ContainsExclusive(
      Rect<int>::FromWidthHeight(5, 15).OffsetToCopy(2, -2)));

  // all are outside
  REQUIRE_FALSE(r.ContainsExclusive(
      Rect<int>::FromWidthHeight(15, 15).OffsetToCopy(-2, -2)));
}


TEST_CASE("rect-insert", "[rect]")
{
  const auto r = Rect<int>::FromWidthHeight(5, 10).InsetCopy(1, 2);

  REQUIRE(r.left == 1);
  REQUIRE(r.right == 4);
  REQUIRE(r.top == 2);
  REQUIRE(r.bottom == 8);
}

TEST_CASE("rect-offset", "[rect]")
{
  const auto r = Rect<int>::FromWidthHeight(5, 10).OffsetCopy(1, 2);

  REQUIRE(r.left == 1);
  REQUIRE(r.right == 6);
  REQUIRE(r.top == 2);
  REQUIRE(r.bottom == 12);
}

TEST_CASE("rect-offset_to", "[rect]")
{
  const auto r =
      Rect<int>::FromTopLeftWidthHeight(10, 12, 5, 10).OffsetToCopy(1, 2);

  REQUIRE(r.left == 1);
  REQUIRE(r.right == 6);
  REQUIRE(r.top == 2);
  REQUIRE(r.bottom == 12);
}

TEST_CASE("rect-empty", "[rect]")
{
  auto r = Rect<int>::FromWidthHeight(5, 10);

  REQUIRE_FALSE(r.IsEmpty());

  r.SetEmpty();

  REQUIRE(r.IsEmpty());
}

TEST_CASE("rect-default_empty", "[rect]")
{
  Rect<int> r;
  REQUIRE(r.IsEmpty());
}

TEST_CASE("rect-sizes", "[rect]")
{
  const auto r = Rect<int>::FromWidthHeight(5, 10);
  REQUIRE(r.GetWidth() == 5);
  REQUIRE(r.GetHeight() == 10);
}

TEST_CASE("rect-center", "[rect]")
{
  const auto r = Rect<int>::FromTopLeftWidthHeight(3, 2, 6, 10);
  REQUIRE(r.GetRelativeCenterX() == 3);
  REQUIRE(r.GetRelativeCenterY() == 5);

  REQUIRE(r.GetAbsoluteCenterX() == 5);
  REQUIRE(r.GetAbsoluteCenterY() == 8);
}
