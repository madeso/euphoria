#include "core/tracery.h"

#include "core/random.h"

#include "catch.hpp"

namespace tr = euphoria::core::tracery;

TEST_CASE("tracery-all", "[tracery]")
{
    const std::string test_file = "test-file";
    auto random = euphoria::core::Random{0};
    tr::Grammar g;
    g.register_english();

    SECTION("empty json")
    {
        const auto loaded = g.load_from_string(test_file, R"(
            <tracery>
            </tracery>
        )");
        REQUIRE(loaded);

        const auto expansion_result = g.flatten(&random, "test");
        REQUIRE(expansion_result);
        CHECK(expansion_result.get_text() == "test");
    }

    SECTION("flatten single rule")
    {
        const auto loaded = g.load_from_string(test_file, R"(
            <tracery>
                <rule name="dog">
                    <text>doggo</text>
                </rule>
            </tracery>
        )");
        REQUIRE(loaded);

        const auto expansion_result = g.flatten(&random, "#dog#");
        REQUIRE(expansion_result);
        CHECK(expansion_result.get_text() == "doggo");
    }

    SECTION("escape")
    {
        const auto loaded = g.load_from_string(test_file, R"(
            <tracery>
                <rule name="esc">
                    <text>\#</text>
                </rule>
            </tracery>
        )");
        REQUIRE(loaded);

        const auto expansion_result = g.flatten(&random, "#esc#");
        REQUIRE(expansion_result);
        CHECK(expansion_result.get_text() == "#");
    }

    SECTION("don't die on non ending rules")
    {
        const auto loaded = g.load_from_string(test_file, R"(
            <tracery>
                <rule name="esc">
                    <text>#dog</text>
                </rule>
            </tracery>
        )");
        REQUIRE(loaded.error_type == tr::Result::rule_eof);
    }

    SECTION("rule call rule")
    {
        const auto loaded = g.load_from_string(test_file, R"(
            <tracery>
                <rule name="dog">
                    <text>doggo</text>
                </rule>
                <rule name="animal">
                    <text>#dog#</text>
                </rule>
            </tracery>
        )");
        REQUIRE(loaded);

        const auto expansion_result = g.flatten(&random, "#animal#");
        REQUIRE(expansion_result);
        CHECK(expansion_result.get_text() == "doggo");
    }

    SECTION("weird name rules")
    {
        const auto loaded = g.load_from_string(test_file, R"(
            <tracery>
                <rule name="animal-dog">
                    <text>doggo</text>
                </rule>
                <rule name="animal">
                    <text>#animal-dog#</text>
                </rule>
            </tracery>
        )");
        REQUIRE(loaded);

        const auto expansion_result = g.flatten(&random, "#animal#");
        REQUIRE(expansion_result);
        CHECK(expansion_result.get_text() == "doggo");
    }

    SECTION("func a")
    {
        const auto loaded = g.load_from_string(test_file, R"(
            <tracery>
                <rule name="dog">
                    <text>dog</text>
                </rule>
                <rule name="animal">
                    <text>#dog.a#</text>
                </rule>
            </tracery>
        )");
        REQUIRE(loaded);

        const auto expansion_result = g.flatten(&random, "#animal#");
        REQUIRE(expansion_result);
        CHECK(expansion_result.get_text() == "a dog");
    }


    SECTION("func s")
    {
        const auto loaded = g.load_from_string(test_file, R"(
            <tracery>
                <rule name="dog">
                    <text>dog</text>
                </rule>
                <rule name="animal">
                    <text>#dog.s#</text>
                </rule>
            </tracery>
        )");
        REQUIRE(loaded);

        const auto expansion_result = g.flatten(&random, "#animal#");
        REQUIRE(expansion_result);
        CHECK(expansion_result.get_text() == "dogs");
    }

    SECTION("push rules")
    {
        const auto loaded = g.load_from_string(test_file, R"(
            <tracery>
                <rule name="dog">
                    <text>dog</text>
                </rule>
                <rule name="name">
                    <text>dog</text>
                </rule>
                <rule name="story">
                    <text>#hero# is good</text>
                </rule>
                <rule name="origin">
                    <text>#[hero:#name#]story#</text>
                </rule>
            </tracery>
        )");
        REQUIRE(loaded);

        const auto expansion_result = g.flatten(&random, "#origin#");
        REQUIRE(expansion_result);
        CHECK(expansion_result.get_text() == "dog is good");
    }

    SECTION("advanced push")
    {
        const auto loaded = g.load_from_string(test_file, R"(
            <tracery>
                <rule name="origin">
                    <text>I love #[animal:#cat#]say# and #[animal:POP]say#</text>
                </rule>
                <rule name="say">
                    <text>#animal.s#</text>
                </rule>
                <rule name="animal">
                    <text>dog</text>
                </rule>
                <rule name="cat">
                    <text>cat</text>
                </rule>
            </tracery>
        )");
        REQUIRE(loaded);

        const auto expansion_result = g.flatten(&random, "#origin#");
        REQUIRE(expansion_result);
        CHECK(expansion_result.get_text() == "I love cats and dogs");
    }
}
