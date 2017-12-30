#include "core/levenshtein.h"

#include "catch.hpp"

TEST_CASE("levenshtein")
{
  const std::string  a        = "kitten";
  const std::string  b        = "sitting";
  cons unsigned long distance = 3;

  CHECK(LevenshteinDistance(a, b) == distance);
  CHECK(LevenshteinDistance(b, a) == distance);
}
