#include "core/quicksort.h"

#include "catch.hpp"

TEST_CASE("quicksort-empty", "[quicksort]")
{
  auto v = std::vector<int>{5, 3, 6, 1};
  QuickSort(&v);
  const auto sorted = std::vector<int>{1, 3, 5, 6};
  CHECK(v == sorted);
}

// todo: add more tests

