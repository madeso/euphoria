#include "core/tracery.h"

#include "base/random.h"

#include "catch.hpp"

namespace tr = eu::core::tracery;

TEST_CASE("tracery-all", "[tracery]")
{
    const std::string test_file = "test-file";
    auto random = eu::Random{0};
    tr::Grammar g;
    g.register_english();

    SECTION("empty json")
    {
        const auto loaded = g.load_from_string(test_file, R"(
            {
            }
        )");
        REQUIRE(loaded);

        const auto expansion_result = g.flatten(&random, "test");
        REQUIRE(expansion_result);
        CHECK(expansion_result.get_text() == "test");
    }

    SECTION("flatten single rule")
    {
        const auto loaded = g.load_from_string(test_file, R"(
            {
                "dog": "doggo"
            }
        )");
        REQUIRE(loaded);

        const auto expansion_result = g.flatten(&random, "#dog#");
        REQUIRE(expansion_result);
        CHECK(expansion_result.get_text() == "doggo");
    }

    SECTION("escape")
    {
        // need double backslashes to escape \ and
        // to actually enter a \ into the json string
        const auto loaded = g.load_from_string(test_file, R"(
            {
                "esc": "\\#"
            }
        )");
        REQUIRE(loaded);

        const auto expansion_result = g.flatten(&random, "#esc#");
        REQUIRE(expansion_result);
        CHECK(expansion_result.get_text() == "#");
    }

    SECTION("don't die on non ending rules")
    {
        // need double backslashes to escape \ and
        // to actually enter a \ into the json string
        const auto loaded = g.load_from_string(test_file, R"(
            {
                "esc": "#dog"
            }
        )");
        REQUIRE(loaded.error_type == tr::Result::rule_eof);
    }

    SECTION("rule call rule")
    {
        const auto loaded = g.load_from_string(test_file, R"(
            {
                "dog": "doggo",
                "animal": "#dog#"
            }
        )");
        REQUIRE(loaded);

        const auto expansion_result = g.flatten(&random, "#animal#");
        REQUIRE(expansion_result);
        CHECK(expansion_result.get_text() == "doggo");
    }

    SECTION("weird name rules")
    {
        const auto loaded = g.load_from_string(test_file, R"(
            {
                "animal-dog": "doggo",
                "animal": "#animal-dog#"
            }
        )");
        REQUIRE(loaded);

        const auto expansion_result = g.flatten(&random, "#animal#");
        REQUIRE(expansion_result);
        CHECK(expansion_result.get_text() == "doggo");
    }

    SECTION("func a")
    {
        const auto loaded = g.load_from_string(test_file, R"(
            {
                "dog": "dog",
                "animal": "#dog.a#"
            }
        )");
        REQUIRE(loaded);

        const auto expansion_result = g.flatten(&random, "#animal#");
        REQUIRE(expansion_result);
        CHECK(expansion_result.get_text() == "a dog");
    }


    SECTION("func s")
    {
        const auto loaded = g.load_from_string(test_file, R"(
            {
                "dog": "dog",
                "animal": "#dog.s#"
            }
        )");
        REQUIRE(loaded);

        const auto expansion_result = g.flatten(&random, "#animal#");
        REQUIRE(expansion_result);
        CHECK(expansion_result.get_text() == "dogs");
    }

    SECTION("push rules")
    {
        const auto loaded = g.load_from_string(test_file, R"(
            {
                "dog": "dog",
                "name": ["dog"],
                "story": ["#hero# is good"],
                "origin": ["#[hero:#name#]story#"]
            }
        )");
        REQUIRE(loaded);

        const auto expansion_result = g.flatten(&random, "#origin#");
        REQUIRE(expansion_result);
        CHECK(expansion_result.get_text() == "dog is good");
    }

    SECTION("advanced push")
    {
        const auto loaded = g.load_from_string(test_file, R"(
            {
                "origin":"I love #[animal:#cat#]say# and #[animal:POP]say#",
                "say":"#animal.s#",
                "animal": "dog",
                "cat":"cat"
            }
        )");
        REQUIRE(loaded);

        const auto expansion_result = g.flatten(&random, "#origin#");
        REQUIRE(expansion_result);
        CHECK(expansion_result.get_text() == "I love cats and dogs");
    }
}
