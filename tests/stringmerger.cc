#include "core/stringmerger.h"

#include "catch.hpp"

#define STRING_VEC(...) std::vector<std::string>{__VA_ARGS__}

TEST_CASE("stringmerger", "[stringmerger]")
{
  const auto empty = STRING_VEC();
  const auto ty = STRING_VEC("t", "y");
  const auto dog = STRING_VEC("dog");
  const auto catdog = STRING_VEC("cat", "dog");
  const auto abc = STRING_VEC("a", "b", "c");

  SECTION("default")
  {
    CHECK(StringMerger::EnglishAnd().Generate(empty) == "<none>");
    CHECK(StringMerger::EnglishAnd().Generate(catdog) == "cat and dog");
    CHECK(StringMerger::EnglishAnd().Generate(abc) == "a, b and c");

    CHECK(StringMerger::EnglishOr().Generate(empty) == "<none>");
    CHECK(StringMerger::EnglishOr().Generate(catdog) == "cat or dog");
    CHECK(StringMerger::EnglishOr().Generate(abc) == "a, b or c");

    CHECK(StringMerger::Array().Generate(empty) == "[]");
    CHECK(StringMerger::Array().Generate(ty) == "[t, y]");
    CHECK(StringMerger::Array().Generate(abc) == "[a, b, c]");

    CHECK(StringMerger::FunctionCall().Generate(empty) == "()");
    CHECK(StringMerger::FunctionCall().Generate(ty) == "(t, y)");
    CHECK(StringMerger::FunctionCall().Generate(abc) == "(a, b, c)");

    CHECK(StringMerger::Space().Generate(empty) == "");
    CHECK(StringMerger::Space().Generate(ty) == "t y");
    CHECK(StringMerger::Space().Generate(abc) == "a b c");

    CHECK(StringMerger::Comma().Generate(empty) == "");
    CHECK(StringMerger::Comma().Generate(ty) == "t, y");
    CHECK(StringMerger::Comma().Generate(abc) == "a, b, c");
  }

  SECTION("custom")
  {
    SECTION("empty constructor")
    {
      const auto m = StringMerger{};
      CHECK(m.Generate(empty) == "");
      CHECK(m.Generate(ty) == "ty");
      CHECK(m.Generate(abc) == "abc");
    }

    SECTION("set start & end to different")
    {
      const auto m = StringMerger{}.SetStartAndEnd("X", "Y");
      CHECK(m.Generate(empty) == "XY");
      CHECK(m.Generate(ty) == "XtyY");
      CHECK(m.Generate(abc) == "XabcY");
    }

    SECTION("set start & end to same")
    {
      const auto m = StringMerger{}.SetStartAndEnd("X");
      CHECK(m.Generate(empty) == "XX");
      CHECK(m.Generate(ty) == "XtyX");
      CHECK(m.Generate(abc) == "XabcX");
    }

    SECTION("set sep")
    {
      const auto m = StringMerger{}.SetSeparator("X");
      CHECK(m.Generate(empty) == "");
      CHECK(m.Generate(ty) == "tXy");
      CHECK(m.Generate(abc) == "aXbXc");
    }

    SECTION("set sep and final")
    {
      const auto m = StringMerger{}.SetSeparator("X", "Y");
      CHECK(m.Generate(empty) == "");
      CHECK(m.Generate(ty) == "tYy");
      CHECK(m.Generate(abc) == "aXbYc");
    }

    SECTION("set empty")
    {
      const auto m = StringMerger{}.SetEmpty("X");
      CHECK(m.Generate(empty) == "X");
      CHECK(m.Generate(ty) == "ty");
      CHECK(m.Generate(abc) == "abc");
    }

    SECTION("before each")
    {
      const auto m = StringMerger{}.SetBeforeEach("X");
      CHECK(m.Generate(empty) == "");
      CHECK(m.Generate(dog) == "Xdog");
      CHECK(m.Generate(ty) == "XtXy");
      CHECK(m.Generate(abc) == "XaXbXc");
    }

    SECTION("after each")
    {
      const auto m = StringMerger{}.SetAfterEach("X");
      CHECK(m.Generate(empty) == "");
      CHECK(m.Generate(dog) == "dogX");
      CHECK(m.Generate(ty) == "tXyX");
      CHECK(m.Generate(abc) == "aXbXcX");
    }

    SECTION("after each and final")
    {
      const auto m = StringMerger{}.SetAfterEach("X", "Y");
      CHECK(m.Generate(empty) == "");
      CHECK(m.Generate(dog) == "dogY");
      CHECK(m.Generate(ty) == "tXyY");
      CHECK(m.Generate(abc) == "aXbXcY");
    }
  }
}
