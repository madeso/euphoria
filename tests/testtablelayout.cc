#include "core/tablelayout.h"

#include "catch.hpp"

template <typename T>
std::vector<T>
MakeVec(std::initializer_list<T> args)
{
  std::vector<T> r{args};
  return r;
}

TEST_CASE("tablelayout-basic", "[tablelayout]")
{
  REQUIRE(PerformTableLayout<int>({3, 2}, 12) == MakeVec({3, 2}));
}

TEST_CASE("tablelayout-one_expand", "[tablelayout]")
{
  REQUIRE(PerformTableLayout<int>({3, -1, 2}, 15) == MakeVec({3, 10, 2}));
}

TEST_CASE("tablelayout-two_expand", "[tablelayout]")
{
  REQUIRE(PerformTableLayout<float>({-1, 3, 2, -1}, 15) ==
          MakeVec({5.0f, 3.0f, 2.0f, 5.0f}));
}

TEST_CASE("tablelayout-min_size", "[tablelayout]")
{
  REQUIRE(PerformTableLayout<int>({5, -1, 5}, 10) == MakeVec({5, 0, 5}));
}

TEST_CASE("tablelayout-min_size_not_default", "[tablelayout]")
{
  REQUIRE(PerformTableLayout<int>({5, -1, 5}, 10, -10) == MakeVec({5, -10, 5}));
}

TEST_CASE("tablelayout-smaller_size", "[tablelayout]")
{
  REQUIRE(PerformTableLayout<float>({5, -1, 5}, 5) ==
          MakeVec({2.5f, 0.0f, 2.5f}));
}

TEST_CASE("tablelayout-smaller_size_not_default", "[tablelayout]")
{
  REQUIRE(PerformTableLayout<float>({5, -1, 5}, 5, -2) ==
          MakeVec({2.5f, -2.0f, 2.5f}));
}
