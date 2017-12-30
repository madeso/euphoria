#include "core/argparse.h"

#include "catch.hpp"

enum MyEnum
{
  MyVal,
  MyVal2
};

TEST_CASE("argparse", "[argparse]")
{
  // test data
  const std::string name  = "APP.exe";
  const auto        empty = std::vector<std::string>{};

  // arguments to be parsed
  std::string              compiler;
  int                      i;
  int                      op = 2;
  std::vector<std::string> strings;
  MyEnum                   enum_value = MyVal;

  // parser setup
  auto parser = argparse::Parser{"description"};
  parser.add_simple("compiler", compiler);
  parser.add_simple("int", i);
  parser.add_simple("-op", op);
  parser.add_vector("-strings", strings).metavar("string");
  const auto convert =
      argparse::Convert<MyEnum>{}("MyVal", MyVal)("MyVal2", MyVal2);
  parser.add_simple<MyEnum>("-enum", enum_value, convert);

  SECTION("test parse")
  {
    const auto arguments = std::vector<std::string>{"gcc", "3"};
    CHECK(argparse::Parser::ParseComplete == parser.parse(name, arguments));
    CHECK(compiler == "gcc");
    CHECK(i == 3);
    CHECK(op == 2);
    CHECK(strings == empty);
    CHECK(enum_value == MyVal);
  }

  SECTION("test enum")
  {
    const auto arguments =
        std::vector<std::string>{"gcc", "3", "-enum", "MyVal2"};
    CHECK(argparse::Parser::ParseComplete == parser.parse(name, arguments));
    CHECK(compiler == "gcc");
    CHECK(i == 3);
    CHECK(op == 2);
    CHECK(strings == empty);
    CHECK(enum_value == MyVal2);
  }

  SECTION("test parse optional")
  {
    const auto arguments = std::vector<std::string>{"gcc", "3", "-op", "42"};
    CHECK(argparse::Parser::ParseComplete == parser.parse(name, arguments));
    CHECK(compiler == "gcc");
    CHECK(i == 3);
    CHECK(op == 42);
    CHECK(strings == empty);
    CHECK(enum_value == MyVal);
  }

  SECTION("test parse multiple")
  {
    const auto arguments =
        std::vector<std::string>{"clang", "5", "-strings", "a", "b", "c"};
    CHECK(argparse::Parser::ParseComplete == parser.parse(name, arguments));
    CHECK(compiler == "clang");
    CHECK(i == 5);
    CHECK(op == 2);
    const auto abc = std::vector<std::string>{"a", "b", "c"};
    CHECK(strings == abc);
    CHECK(enum_value == MyVal);
  }
}
