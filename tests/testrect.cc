#include "core/rect.h"

#include "catch.hpp"

// constructor tests

namespace euco = euphoria::core;

TEST_CASE("rect-constructor_topleftwidthheight", "[rect]")
{
  auto r = euco::Rect<int>::FromTopLeftWidthHeight(2, 1, 3, 4);

  CHECK(r.left == 1);
  CHECK(r.top == 2);

  CHECK(r.right == 4);    // left: 1, width_: 3
  CHECK(r.bottom == -2);  // top: 2, height: 4

  CHECK(r.GetWidth() == 3);
  CHECK(r.GetHeight() == 4);

  REQUIRE_FALSE(r.IsEmpty());
}

TEST_CASE("rect-offsetcopy", "[rect]")
{
  auto r = euco::Rect<int>::FromWidthHeight(3, 4).SetTopLeftToCopy(1, 2);

  CHECK(r.left == 1);
  CHECK(r.top == 2);

  CHECK(r.right == 4);    // left: 1, width_: 3
  CHECK(r.bottom == -2);  // top: 2, height: 4

  CHECK(r.GetWidth() == 3);
  CHECK(r.GetHeight() == 4);

  REQUIRE_FALSE(r.IsEmpty());
}

TEST_CASE("rect-constructor_widthheight", "[rect]")
{
  auto r = euco::Rect<int>::FromWidthHeight(3, 4);

  CHECK(r.left == 0);
  CHECK(r.right == 3);
  CHECK(r.top == 4);
  CHECK(r.bottom == 0);

  CHECK(r.GetWidth() == 3);
  CHECK(r.GetHeight() == 4);

  REQUIRE_FALSE(r.IsEmpty());
}


TEST_CASE("rect-constructor_leftrighttopbottom", "[rect]")
{
  auto r = euco::Rect<int>::FromLeftRightBottomTop(1, 2, 3, 4);

  REQUIRE(r.left == 1);
  REQUIRE(r.right == 2);
  REQUIRE(r.bottom == 3);
  REQUIRE(r.top == 4);

  REQUIRE_FALSE(r.IsEmpty());
}

// operation tests

TEST_CASE("rect-contains_point_exclusive", "[rect]")
{
  const auto r = euco::Rect<int>::FromWidthHeight(5, 5);
  REQUIRE_FALSE(r.IsEmpty());
  REQUIRE(r.IsValid());

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
  const auto r = euco::Rect<int>::FromWidthHeight(5, 5);

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
  const auto r = euco::Rect<int>::FromWidthHeight(10, 10);

  // inside
  REQUIRE(r.ContainsExclusive(
      euco::Rect<int>::FromWidthHeight(2, 2).SetTopLeftToCopy(3, 3)));

  // outside negative
  REQUIRE_FALSE(r.ContainsExclusive(
      euco::Rect<int>::FromWidthHeight(2, 2).SetTopLeftToCopy(-3, 3)));
  REQUIRE_FALSE(r.ContainsExclusive(
      euco::Rect<int>::FromWidthHeight(2, 2).SetTopLeftToCopy(3, -3)));
  REQUIRE_FALSE(r.ContainsExclusive(
      euco::Rect<int>::FromWidthHeight(2, 2).SetTopLeftToCopy(-3, -3)));

  // outside positive
  REQUIRE_FALSE(r.ContainsExclusive(
      euco::Rect<int>::FromWidthHeight(2, 2).SetTopLeftToCopy(15, 3)));
  REQUIRE_FALSE(r.ContainsExclusive(
      euco::Rect<int>::FromWidthHeight(2, 2).SetTopLeftToCopy(3, 15)));
  REQUIRE_FALSE(r.ContainsExclusive(
      euco::Rect<int>::FromWidthHeight(2, 2).SetTopLeftToCopy(15, 15)));

  // over border
  REQUIRE_FALSE(r.ContainsExclusive(
      euco::Rect<int>::FromWidthHeight(15, 5).SetTopLeftToCopy(-2, 2)));
  REQUIRE_FALSE(r.ContainsExclusive(
      euco::Rect<int>::FromWidthHeight(5, 15).SetTopLeftToCopy(2, -2)));

  // all are outside
  REQUIRE_FALSE(r.ContainsExclusive(
      euco::Rect<int>::FromWidthHeight(15, 15).SetTopLeftToCopy(-2, -2)));
}

TEST_CASE("rect-inset", "[rect]")
{
  const auto r = euco::Rect<int>::FromWidthHeight(5, 10).InsetCopy(1, 2);

  REQUIRE(r.left == 1);
  REQUIRE(r.right == 4);
  REQUIRE(r.top == 8);
  REQUIRE(r.bottom == 2);
}

TEST_CASE("rect-inset-different", "[rect]")
{
  const auto r = euco::Rect<int>::FromWidthHeight(5, 10).InsetCopy(1, 2, 3, 4);

  REQUIRE(r.left == 1);
  REQUIRE(r.right == 3);
  REQUIRE(r.top == 7);
  REQUIRE(r.bottom == 4);
}

TEST_CASE("rect-offset", "[rect]")
{
  const auto r = euco::Rect<int>::FromWidthHeight(5, 10).OffsetCopy(1, 2);

  REQUIRE(r.left == 1);
  REQUIRE(r.right == 6);
  REQUIRE(r.top == 12);
  REQUIRE(r.bottom == 2);
}

TEST_CASE("rect-offset_to", "[rect]")
{
  const auto r =
      euco::Rect<int>::FromTopLeftWidthHeight(10, 12, 5, 10).SetTopLeftToCopy(1, 2);

  REQUIRE(r.IsValid());

  REQUIRE(r.left == 1);
  REQUIRE(r.right == 6);
  REQUIRE(r.top == 2);
  REQUIRE(r.bottom == -8);
}

TEST_CASE("rect-empty", "[rect]")
{
  auto r = euco::Rect<int>::FromWidthHeight(5, 10);

  REQUIRE_FALSE(r.IsEmpty());

  r.SetEmpty();

  REQUIRE(r.IsEmpty());
}

TEST_CASE("rect-default_empty", "[rect]")
{
  euco::Rect<int> r;
  REQUIRE(r.IsEmpty());
}

TEST_CASE("rect-sizes", "[rect]")
{
  const auto r = euco::Rect<int>::FromWidthHeight(5, 10);
  REQUIRE(r.GetWidth() == 5);
  REQUIRE(r.GetHeight() == 10);
}

TEST_CASE("rect-center", "[rect]")
{
  const auto r = euco::Rect<int>::FromTopLeftWidthHeight(3, 2, 6, 10);
  REQUIRE(r.GetRelativeCenterXFromBottomLeft() == 3);
  REQUIRE(r.GetRelativeCenterYFromBottomLeft() == 5);

  REQUIRE(r.GetAbsoluteCenterX() == 5);
  REQUIRE(r.GetAbsoluteCenterY() == -2);  // at y=3 and the rect is 10
}

TEST_CASE("rect-from-anchor", "[rect]")
{
  const int     height = 3;
  const int     width  = 4;
  const euco::vec2i origo  = euco::vec2i::Zero();

  SECTION("lower left at origo")
  {
    const auto r = euco::Recti::FromPositionAnchorWidthAndHeight(
        origo, euco::scale2i{0, 0}, width, height);
    REQUIRE(r.left == 0);
    REQUIRE(r.right == width);

    REQUIRE(r.top == height);
    REQUIRE(r.bottom == 0);
  }

  SECTION("upper right at origo")
  {
    const auto r = euco::Recti::FromPositionAnchorWidthAndHeight(
        origo, euco::scale2i{1, 1}, width, height);
    REQUIRE(r.left == -width);
    REQUIRE(r.right == 0);

    REQUIRE(r.top == 0);
    REQUIRE(r.bottom == -height);
  }
}

TEST_CASE("rect-from-anchor-center", "[rect]")
{
  const float   half_height = 3;
  const float   half_width  = 4;
  const euco::vec2f origo       = euco::vec2f::Zero();

  SECTION("lower left at origo")
  {
    const auto r = euco::Rectf::FromPositionAnchorWidthAndHeight(
        origo, euco::scale2f{0.5f, 0.5f}, half_width * 2, half_height * 2);
    REQUIRE(r.left == Approx{-half_width});
    REQUIRE(r.right == Approx{half_width});

    REQUIRE(r.top == Approx{half_height});
    REQUIRE(r.bottom == Approx{-half_height});
  }
}
