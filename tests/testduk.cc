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

  SECTION("print tests")
  {
    std::string print_result;
    duk.bind_print([&](const std::string& str) { print_result = str; });

    CAPTURE(error);
    CAPTURE(out);
    SECTION("print int")
    {
      REQUIRE(duk.eval_string("print(3);", "", &error, &out));
      REQUIRE(print_result == "3");
    }

    SECTION("print double")
    {
      REQUIRE(duk.eval_string("print(4.2);", "", &error, &out));
      REQUIRE(print_result == "4.2");
    }

    SECTION("print string")
    {
      REQUIRE(duk.eval_string("print(\"cat\");", "", &error, &out));
      REQUIRE(print_result == "cat");
    }

    SECTION("print int")
    {
      REQUIRE(duk.eval_string("print(3);", "", &error, &out));
      REQUIRE(print_result == "3");
    }

    SECTION("print int & double")
    {
      REQUIRE(duk.eval_string("print(1, 4.2);", "", &error, &out));
      REQUIRE(print_result == "1 4.2");
    }

    SECTION("print string int & double")
    {
      REQUIRE(duk.eval_string("print(\"hat\", 1, 4.2);", "", &error, &out));
      REQUIRE(print_result == "hat 1 4.2");
    }

    SECTION("print var")
    {
      REQUIRE(duk.eval_string(
          "hat = \"cat\"; print(hat, 1, 4.2);", "", &error, &out));
      REQUIRE(print_result == "cat 1 4.2");
    }
  }

  SECTION("custom function")
  {
// needs improvement
#if 0
    int value = 12;
    FunctionBinder{&duk, "test"}.bind<int>([&](Context* ctx, int i) -> int {
      value = i;
      return 0;
    });
    REQUIRE(value == 12);
    REQUIRE(duk.eval_string("test(42);", "", &error, &out));
    REQUIRE(value == 42);
#endif
  }
}
