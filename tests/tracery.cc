#include "core/tracery.h"

#include "catch.hpp"

TEST_CASE("tracery-all", "[tracery]")
{
  Grammar g;
  g.RegisterEnglish();

  SECTION("empty json")
  {
    const auto loaded = g.LoadFromString(R"(
      {
      }
    )");
    REQUIRE(loaded);

    const auto expansion_result = g.Flatten("test");
    REQUIRE(expansion_result);
    CHECK(expansion_result.GetText() == "test");
  }

  SECTION("flatten single rule")
  {
    const auto loaded = g.LoadFromString(R"(
      {
        "dog": "doggo"
      }
    )");
    REQUIRE(loaded);

    const auto expansion_result = g.Flatten("#dog#");
    REQUIRE(expansion_result);
    CHECK(expansion_result.GetText() == "doggo");
  }

  SECTION("escape")
  {
    // need double backslashes to escape \ and 
    // to actually enter a \ into the json string
    const auto loaded = g.LoadFromString(R"(
      {
        "esc": "\\#"
      }
    )");
    REQUIRE(loaded);

    const auto expansion_result = g.Flatten("#esc#");
    REQUIRE(expansion_result);
    CHECK(expansion_result.GetText() == "#");
  }

  SECTION("don't die on non ending rules")
  {
    // need double backslashes to escape \ and 
    // to actually enter a \ into the json string
    const auto loaded = g.LoadFromString(R"(
      {
        "esc": "#"
      }
    )");
    REQUIRE(loaded.type == Result::RULE_EOF);
  }

  SECTION("rule call rule")
  {
    const auto loaded = g.LoadFromString(R"(
      {
        "dog": "doggo",
        "animal": "#dog#"
      }
    )");
    REQUIRE(loaded);

    const auto expansion_result = g.Flatten("#animal#");
    REQUIRE(expansion_result);
    CHECK(expansion_result.GetText() == "doggo");
  }
}

