#include "core/commandlineparser.h"

#include "catch.hpp"

TEST_CASE("commandline", "[commandline]")
{
  SECTION("empty cmd")
  {
    CHECK(std::vector<std::string>{} == ParseCommandLine(""));
  }

  SECTION("single cmd")
  {
    CHECK(std::vector<std::string>{"dog"} == ParseCommandLine("dog"));
  }

  SECTION("arguments")
  {
    const auto vec = std::vector<std::string>{"dog", "cat", "cat"};
    CHECK(vec == ParseCommandLine("dog cat cat"));
  }

  SECTION("with backslash")
  {
    const auto vec = std::vector<std::string>{"dog", "cat\\dog"};
    CHECK(vec == ParseCommandLine("dog cat\\\\dog"));
  }

  SECTION("several spaces")
  {
    const auto vec = std::vector<std::string>{"dog", "cat", "cat"};
    CHECK(vec == ParseCommandLine("   dog    cat   cat   "));
  }

  SECTION("todo different types of spaces")
  {
    const auto vec = std::vector<std::string>{"dog", "cat", "cat"};
    CHECK(vec == ParseCommandLine("\tdog\tcat\tcat\t"));
  }

  SECTION("escapes")
  {
    const auto vec = std::vector<std::string>{"dog", "\n", "\t"};
    CHECK(vec == ParseCommandLine("dog \\n \\t"));
  }

  SECTION("escapes without space")
  {
    const auto vec = std::vector<std::string>{"dog", "cat", "cat"};
    CHECK(vec == ParseCommandLine("dog cat \"cat\""));
  }

  SECTION("escapes with space")
  {
    const auto vec = std::vector<std::string>{"dog", "cat", "cat and dog"};
    CHECK(vec == ParseCommandLine("dog cat \"cat and dog\""));
  }

  SECTION("escapes with missing quote")
  {
    const auto vec = std::vector<std::string>{"dog", "cat", "cat and dog"};
    CHECK(vec == ParseCommandLine("dog cat \"cat and dog"));
  }

  SECTION("escapes with quotes")
  {
    const auto vec = std::vector<std::string>{"dog", "cat", "cat and \"dog\""};
    CHECK(vec == ParseCommandLine("dog cat \"cat and \\\"dog\\\"\""));
  }

  SECTION("escapes without quotes")
  {
    const auto vec = std::vector<std::string>{"dog", "\"cat\""};
    CHECK(vec == ParseCommandLine("dog \\\"cat\\\""));
  }

  SECTION("end with escape")
  {
    const auto vec = std::vector<std::string>{"dog", "\"cat"};
    CHECK(vec == ParseCommandLine("dog \\\"cat\\"));
  }

  SECTION("string end with escape")
  {
    const auto vec = std::vector<std::string>{"dog", "cat"};
    CHECK(vec == ParseCommandLine("dog \"cat\\"));
  }

  SECTION("single quote strings")
  {
    const auto vec = std::vector<std::string>{"dog", "cat and dog"};
    CHECK(vec == ParseCommandLine("dog 'cat and dog'"));
  }

  SECTION("single quote in double quote")
  {
    const auto vec = std::vector<std::string>{"dog", "'cat'"};
    CHECK(vec == ParseCommandLine("dog \"'cat'\""));
  }

  SECTION("double quote in single quote")
  {
    const auto vec = std::vector<std::string>{"dog", "\"cat\""};
    CHECK(vec == ParseCommandLine("dog '\"cat\"'"));
  }
}
