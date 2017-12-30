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

  // parser setup
  auto parser = argparse::Parser{"description"};
  parser.add_simple("compiler", compiler);
  parser.add_simple("int", i);
  parser.add_simple("-op", op);
  parser.add_vector("-strings", strings).metavar("string");
  //("-enum", &v, Convert<MyEnum>("MyVal", MyEnum::MyVal)("MyVal2",
  // MyEnum::MyVal2) )


  SECTION("test parse")
  {
    const auto arguments = std::vector<std::string>{"gcc", "3"};
    CHECK(argparse::Parser::ParseComplete == parser.parse(name, arguments));
    CHECK(compiler == "gcc");
    CHECK(i == 3);
    CHECK(op == 2);
    CHECK(strings == empty);
  }

  SECTION("test parse optional")
  {
    const auto arguments = std::vector<std::string>{"gcc", "3", "-op", "42"};
    CHECK(argparse::Parser::ParseComplete == parser.parse(name, arguments));
    CHECK(compiler == "gcc");
    CHECK(i == 3);
    CHECK(op == 42);
    CHECK(strings == empty);
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
  }
}
