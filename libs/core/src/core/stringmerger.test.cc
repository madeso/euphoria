#include "core/stringmerger.h"

#include "catch.hpp"

namespace euco = eu::core;


TEST_CASE("stringmerger", "[stringmerger]")
{
    const auto empty = std::vector<std::string> { };
    const auto ty = std::vector<std::string> { "t", "y" };
    const auto dog = std::vector<std::string> { "dog" };
    const auto catdog = std::vector<std::string> { "cat", "dog" };
    const auto abc = std::vector<std::string> { "a", "b", "c" };

    SECTION("default")
    {
        CHECK(euco::string_mergers::english_and.merge(empty) == "<none>");
        CHECK(euco::string_mergers::english_and.merge(catdog) == "cat and dog");
        CHECK(euco::string_mergers::english_and.merge(abc) == "a, b and c");

        CHECK(euco::string_mergers::english_or.merge(empty) == "<none>");
        CHECK(euco::string_mergers::english_or.merge(catdog) == "cat or dog");
        CHECK(euco::string_mergers::english_or.merge(abc) == "a, b or c");

        CHECK(euco::string_mergers::array.merge(empty) == "[]");
        CHECK(euco::string_mergers::array.merge(ty) == "[t, y]");
        CHECK(euco::string_mergers::array.merge(abc) == "[a, b, c]");

        CHECK(euco::string_mergers::function_call.merge(empty) == "()");
        CHECK(euco::string_mergers::function_call.merge(ty) == "(t, y)");
        CHECK(euco::string_mergers::function_call.merge(abc) == "(a, b, c)");

        CHECK(euco::string_mergers::space.merge(empty).empty());
        CHECK(euco::string_mergers::space.merge(ty) == "t y");
        CHECK(euco::string_mergers::space.merge(abc) == "a b c");

        CHECK(euco::string_mergers::comma.merge(empty).empty());
        CHECK(euco::string_mergers::comma.merge(ty) == "t, y");
        CHECK(euco::string_mergers::comma.merge(abc) == "a, b, c");
    }

    SECTION("custom")
    {
        SECTION("empty constructor")
        {
            const auto m = euco::StringMerger {};
            CHECK(m.merge(empty).empty());
            CHECK(m.merge(ty) == "ty");
            CHECK(m.merge(abc) == "abc");
        }

        SECTION("set start & end to different")
        {
            const auto m = euco::StringMerger {}.set_start_and_end("X", "Y");
            CHECK(m.merge(empty) == "XY");
            CHECK(m.merge(ty) == "XtyY");
            CHECK(m.merge(abc) == "XabcY");
        }

        SECTION("set start & end to same")
        {
            const auto m = euco::StringMerger {}.set_start_and_end("X");
            CHECK(m.merge(empty) == "XX");
            CHECK(m.merge(ty) == "XtyX");
            CHECK(m.merge(abc) == "XabcX");
        }

        SECTION("set sep")
        {
            const auto m = euco::StringMerger {}.set_separator("X");
            CHECK(m.merge(empty).empty());
            CHECK(m.merge(ty) == "tXy");
            CHECK(m.merge(abc) == "aXbXc");
        }

        SECTION("set sep and final")
        {
            const auto m = euco::StringMerger {}.set_separator("X", "Y");
            CHECK(m.merge(empty).empty());
            CHECK(m.merge(ty) == "tYy");
            CHECK(m.merge(abc) == "aXbYc");
        }

        SECTION("set empty")
        {
            const auto m = euco::StringMerger {}.set_empty("X");
            CHECK(m.merge(empty) == "X");
            CHECK(m.merge(ty) == "ty");
            CHECK(m.merge(abc) == "abc");
        }

        SECTION("before each")
        {
            const auto m = euco::StringMerger {}.set_before_each("X");
            CHECK(m.merge(empty).empty());
            CHECK(m.merge(dog) == "Xdog");
            CHECK(m.merge(ty) == "XtXy");
            CHECK(m.merge(abc) == "XaXbXc");
        }

        SECTION("after each")
        {
            const auto m = euco::StringMerger {}.set_after_each("X");
            CHECK(m.merge(empty).empty());
            CHECK(m.merge(dog) == "dogX");
            CHECK(m.merge(ty) == "tXyX");
            CHECK(m.merge(abc) == "aXbXcX");
        }

        SECTION("after each and final")
        {
            const auto m = euco::StringMerger {}.set_after_each("X", "Y");
            CHECK(m.merge(empty).empty());
            CHECK(m.merge(dog) == "dogY");
            CHECK(m.merge(ty) == "tXyY");
            CHECK(m.merge(abc) == "aXbXcY");
        }
    }
}
