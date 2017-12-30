#include "core/levenshtein.h"

#include "catch.hpp"

TEST_CASE("levenshtein")
{
  const std::string   a        = "kitten";
  const std::string   b        = "sitting";
  const unsigned long distance = 3;

  CHECK(LevenshteinDistance(a, b) == distance);
  CHECK(LevenshteinDistance(b, a) == distance);
}

TEST_CASE("levenshtein2")
{
  const std::string   a        = "abc";
  const std::string   b        = "abcdef";
  const unsigned long distance = 3;

  CHECK(LevenshteinDistance(a, b) == distance);
  CHECK(LevenshteinDistance(b, a) == distance);
}
