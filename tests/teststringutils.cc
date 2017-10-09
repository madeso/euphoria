#include "core/stringutils.h"
#include "catch.hpp"

TEST_CASE("stringutils-laststrings-basic", "[stringutils]")
{
  const auto r = LastStrings("hello.world", '.');
  CHECK(r.first == "hello");
  CHECK(r.second == ".world");
}

TEST_CASE("stringutils-laststrings-last", "[stringutils]")
{
  const auto r = LastStrings("hello.", '.');
  CHECK(r.first == "hello");
  CHECK(r.second == ".");
}

TEST_CASE("stringutils-laststrings-empty", "[stringutils]")
{
  const auto r = LastStrings("hello_world", '.');
  CHECK(r.first == "hello_world");
  CHECK(r.second == "");
}

////////////////////////////////////////////////////////////////////////////////

TEST_CASE("stringutils-striplast", "[stringutils]")
{
  CHECK("hello" == StripLastString("hello.world", '.'));
  CHECK("" == StripLastString("hello_world", '.'));
}
