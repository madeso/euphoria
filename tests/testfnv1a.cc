#include "core/fnv1a.h"

#include "catch.hpp"

TEST_CASE("fnv1a-appendix-c", "[fnv1a]")
{
  // https://tools.ietf.org/html/draft-eastlake-fnv-07#page-9
  REQUIRE(fnv1a("") == 0xcbf29ce484222325);
  REQUIRE(fnv1a("a") == 0xaf63dc4c8601ec8c);
  REQUIRE(fnv1a("foobar") == 0x85944171f73967e8);
}