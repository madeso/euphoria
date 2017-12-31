#include "core/levenshtein.h"

#include "catch.hpp"

TEST_CASE("levenshtein")
{
  const std::string   a        = "kitten";
  const std::string   b        = "sitting";
  const unsigned long distance = 3;

  CHECK(FastLevenshteinDistance(a, b) == distance);
  CHECK(FastLevenshteinDistance(b, a) == distance);

  CHECK(LevenshteinDistance(a, b) == distance);
  CHECK(LevenshteinDistance(b, a) == distance);
}

TEST_CASE("levenshtein2")
{
  const std::string   a        = "abc";
  const std::string   b        = "abcde";
  const unsigned long distance = 2;

  CHECK(FastLevenshteinDistance(a, b) == distance);
  CHECK(FastLevenshteinDistance(b, a) == distance);

  CHECK(LevenshteinDistance(a, b) == distance);
  CHECK(LevenshteinDistance(b, a) == distance);
}

TEST_CASE("levenshtein-codeproject")
{
  const std::string   a        = "GUMBO";
  const std::string   b        = "GAMBOL";
  const unsigned long distance = 2;

  CHECK(FastLevenshteinDistance(a, b) == distance);
  CHECK(FastLevenshteinDistance(b, a) == distance);

  CHECK(LevenshteinDistance(a, b) == distance);
  CHECK(LevenshteinDistance(b, a) == distance);
}
