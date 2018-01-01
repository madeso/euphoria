#include "core/argparse.h"
#include "core/levenshtein.h"

#include "catch.hpp"

enum class MyEnum
{
  MyVal,
  MyVal2
};

/*
TEST_CASE("argparse-help", "[argparse]")
{
  auto       parser    = argparse::Parser{"description"};
  const auto arguments = std::vector<std::string>{"-h"};
  const auto stat      = parser.parse("APP.exe", arguments);
  CHECK(stat.result == argparse::ParseStatus::Complete);

  const std::string usage =
      "usage: APP.exe [-h ]\n"
      "\n"
      "description\n"
      "\n"
      "optional arguments:\n"
      "  -h\tshow this help message and exit.\n"
      "\n";
  INFO("Edit dist: " << FastLevenshteinDistance(stat.out, usage));
  CHECK_THAT(stat.out, Catch::Equals(usage));
  CHECK(stat.error == "");
}
*/

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
  MyEnum                   enum_value = MyEnum::MyVal;

  // parser setup
  auto parser = argparse::Parser{"description"};
  parser.add_simple("compiler", compiler);
  parser.add_simple("int", i);
  parser.add_simple("-op", op);
  parser.add_vector("-strings", strings).metavar("string");
  const auto convert = argparse::Convert<MyEnum>{}("MyVal", MyEnum::MyVal)(
      "MyVal2", MyEnum::MyVal2);
  parser.add_simple<MyEnum>("-enum", enum_value, convert);

  SECTION("test parse")
  {
    const auto arguments = std::vector<std::string>{"gcc", "3"};
    const auto stat      = parser.parse(name, arguments);
    CHECK(stat.result == argparse::ParseStatus::Complete);
    CHECK(stat.out == "");
    CHECK(stat.error == "");
    CHECK(compiler == "gcc");
    CHECK(i == 3);
    CHECK(op == 2);
    CHECK(strings == empty);
    CHECK(enum_value == MyEnum::MyVal);
  }

  SECTION("test enum")
  {
    const auto arguments =
        std::vector<std::string>{"gcc", "3", "-enum", "MyVal2"};
    const auto stat = parser.parse(name, arguments);
    CHECK(stat.result == argparse::ParseStatus::Complete);
    CHECK(stat.out == "");
    CHECK(stat.error == "");
    CHECK(compiler == "gcc");
    CHECK(i == 3);
    CHECK(op == 2);
    CHECK(strings == empty);
    CHECK(enum_value == MyEnum::MyVal2);
  }

  SECTION("test parse optional")
  {
    const auto arguments = std::vector<std::string>{"gcc", "3", "-op", "42"};
    const auto stat      = parser.parse(name, arguments);
    CHECK(stat.result == argparse::ParseStatus::Complete);
    CHECK(stat.out == "");
    CHECK(stat.error == "");
    CHECK(compiler == "gcc");
    CHECK(i == 3);
    CHECK(op == 42);
    CHECK(strings == empty);
    CHECK(enum_value == MyEnum::MyVal);
  }

  SECTION("test parse multiple")
  {
    const auto arguments =
        std::vector<std::string>{"clang", "5", "-strings", "a", "b", "c"};
    const auto stat = parser.parse(name, arguments);
    CHECK(stat.result == argparse::ParseStatus::Complete);
    CHECK(stat.out == "");
    CHECK(stat.error == "");
    CHECK(compiler == "clang");
    CHECK(i == 5);
    CHECK(op == 2);
    const auto abc = std::vector<std::string>{"a", "b", "c"};
    CHECK(strings == abc);
    CHECK(enum_value == MyEnum::MyVal);
  }
}
