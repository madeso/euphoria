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
        "esc": "#dog"
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

  SECTION("weird name rules")
  {
    const auto loaded = g.LoadFromString(R"(
      {
        "animal-dog": "doggo",
        "animal": "#animal-dog#"
      }
    )");
    REQUIRE(loaded);

    const auto expansion_result = g.Flatten("#animal#");
    REQUIRE(expansion_result);
    CHECK(expansion_result.GetText() == "doggo");
  }

  SECTION("func a")
  {
    const auto loaded = g.LoadFromString(R"(
      {
        "dog": "dog",
        "animal": "#dog.a#"
      }
    )");
    REQUIRE(loaded);

    const auto expansion_result = g.Flatten("#animal#");
    REQUIRE(expansion_result);
    CHECK(expansion_result.GetText() == "a dog");
  }


  SECTION("func s")
  {
    const auto loaded = g.LoadFromString(R"(
      {
        "dog": "dog",
        "animal": "#dog.s#"
      }
    )");
    REQUIRE(loaded);

    const auto expansion_result = g.Flatten("#animal#");
    REQUIRE(expansion_result);
    CHECK(expansion_result.GetText() == "dogs");
  }

  SECTION("push rules")
  {
    const auto loaded = g.LoadFromString(R"(
      {
        "dog": "dog",
          "name": ["dog"]
        ,	"story": ["#hero# is good"]
        ,	"origin": ["#[hero:#name#]story#"]
      }
    )");
    REQUIRE(loaded);

    const auto expansion_result = g.Flatten("#origin#");
    REQUIRE(expansion_result);
    CHECK(expansion_result.GetText() == "dog is good");
  }

  SECTION("advanced push")
  {
    const auto loaded = g.LoadFromString(R"(
      {
        "origin":"I love #[animal:#cat#]say# and #[animal:POP]say#",
        "say":"#animal.s#",
        "animal": "dog",
        "cat":"cat"
      }
    )");
    REQUIRE(loaded);

    const auto expansion_result = g.Flatten("#origin#");
    REQUIRE(expansion_result);
    CHECK(expansion_result.GetText() == "I love cats and dogs");
  }
}

