#include "core/duk.h"

#include "catch.hpp"

TEST_CASE("duk-eval", "[duk]")
{
  Duk         duk;
  std::string error;
  std::string out;

  SECTION("empty")
  {
    REQUIRE(duk.eval_string("", "", &error, &out));
  }

  SECTION("test int")
  {
    REQUIRE(duk.eval_string("a = 3; a", "", &error, &out));
    REQUIRE(out == "3");
  }

  SECTION("test float")
  {
    REQUIRE(duk.eval_string("a = 3.3; a", "", &error, &out));
    REQUIRE(out == "3.3");
  }

  SECTION("test string")
  {
    REQUIRE(duk.eval_string("a = \"dog\"; a", "", &error, &out));
    REQUIRE(out == "dog");
  }

  SECTION("object")
  {
    REQUIRE(duk.eval_string("a = new Object(); a", "", &error, &out));
    // test output
    // REQUIRE(out == "aaa");
  }

  SECTION("function")
  {
    REQUIRE(duk.eval_string("a = function() {}; a", "", &error, &out));
    // test output
    // REQUIRE(out == "aaa");
  }
}
