#include "core/editdistance.h"

#include "catch.hpp"

TEST_CASE("editdistance", "[editdistance]")
{
  const std::string   a        = "kitten";
  const std::string   b        = "sitting";
  const unsigned long distance = 3;

  CHECK(FastEditDistance(a, b) == distance);
  CHECK(FastEditDistance(b, a) == distance);

  CHECK(EditDistance(a, b) == distance);
  CHECK(EditDistance(b, a) == distance);
}

TEST_CASE("editdistance2", "[editdistance]")
{
  const std::string   a        = "abc";
  const std::string   b        = "abcde";
  const unsigned long distance = 2;

  CHECK(FastEditDistance(a, b) == distance);
  CHECK(FastEditDistance(b, a) == distance);

  CHECK(EditDistance(a, b) == distance);
  CHECK(EditDistance(b, a) == distance);
}

TEST_CASE("editdistance-codeproject", "[editdistance]")
{
  const std::string   a        = "GUMBO";
  const std::string   b        = "GAMBOL";
  const unsigned long distance = 2;

  CHECK(FastEditDistance(a, b) == distance);
  CHECK(FastEditDistance(b, a) == distance);

  CHECK(EditDistance(a, b) == distance);
  CHECK(EditDistance(b, a) == distance);
}

