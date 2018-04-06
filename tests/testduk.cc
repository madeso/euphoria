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

  SECTION("custom functions")
  {
    SECTION("test(int)")
    {
      // needs improvement
      int value = 12;
      FunctionBinder{&duk, "test"}.bind<int>([&](Context* ctx, int i) -> int {
        value = i;
        return 0;
      });
      REQUIRE(value == 12);
      REQUIRE(duk.eval_string("test(42);", "", &error, &out));
      REQUIRE(value == 42);
    }

    SECTION("test(int, int)")
    {
      // needs improvement
      int value1 = 1;
      int value2 = 2;
      FunctionBinder{&duk, "test"}.bind<int, int>(
          [&](Context* ctx, int a, int b) -> int {
            value1 = a;
            value2 = b;
            return 0;
          });
      REQUIRE(value1 == 1);
      REQUIRE(value2 == 2);
      REQUIRE(duk.eval_string("test(11, 22);", "", &error, &out));
      REQUIRE(value1 == 11);
      REQUIRE(value2 == 22);
    }

    SECTION("test(string)")
    {
      std::string value = "";
      FunctionBinder{&duk, "test"}.bind<std::string>(
          [&](Context* ctx, const std::string& s) -> int {
            value = s;
            return 0;
          });
      REQUIRE(value == "");
      REQUIRE(duk.eval_string("test(\"dog\");", "", &error, &out));
      REQUIRE(value == "dog");
    }
  }
}
