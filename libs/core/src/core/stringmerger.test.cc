#include "core/stringmerger.h"

#include "catch.hpp"

namespace euco = euphoria::core;

#define STRING_VEC(...)                                                        \
    std::vector<std::string>                                                   \
    {                                                                          \
        __VA_ARGS__                                                            \
    }

TEST_CASE("stringmerger", "[stringmerger]")
{
    const auto empty  = STRING_VEC();
    const auto ty     = STRING_VEC("t", "y");
    const auto dog    = STRING_VEC("dog");
    const auto catdog = STRING_VEC("cat", "dog");
    const auto abc    = STRING_VEC("a", "b", "c");

    SECTION("default")
    {
        CHECK(euco::StringMerger::EnglishAnd().Generate(empty) == "<none>");
        CHECK(euco::StringMerger::EnglishAnd().Generate(catdog)
              == "cat and dog");
        CHECK(euco::StringMerger::EnglishAnd().Generate(abc) == "a, b and c");

        CHECK(euco::StringMerger::EnglishOr().Generate(empty) == "<none>");
        CHECK(euco::StringMerger::EnglishOr().Generate(catdog) == "cat or dog");
        CHECK(euco::StringMerger::EnglishOr().Generate(abc) == "a, b or c");

        CHECK(euco::StringMerger::Array().Generate(empty) == "[]");
        CHECK(euco::StringMerger::Array().Generate(ty) == "[t, y]");
        CHECK(euco::StringMerger::Array().Generate(abc) == "[a, b, c]");

        CHECK(euco::StringMerger::FunctionCall().Generate(empty) == "()");
        CHECK(euco::StringMerger::FunctionCall().Generate(ty) == "(t, y)");
        CHECK(euco::StringMerger::FunctionCall().Generate(abc) == "(a, b, c)");

        CHECK(euco::StringMerger::Space().Generate(empty).empty());
        CHECK(euco::StringMerger::Space().Generate(ty) == "t y");
        CHECK(euco::StringMerger::Space().Generate(abc) == "a b c");

        CHECK(euco::StringMerger::Comma().Generate(empty).empty());
        CHECK(euco::StringMerger::Comma().Generate(ty) == "t, y");
        CHECK(euco::StringMerger::Comma().Generate(abc) == "a, b, c");
    }

    SECTION("custom")
    {
        SECTION("empty constructor")
        {
            const auto m = euco::StringMerger {};
            CHECK(m.Generate(empty).empty());
            CHECK(m.Generate(ty) == "ty");
            CHECK(m.Generate(abc) == "abc");
        }

        SECTION("set start & end to different")
        {
            const auto m = euco::StringMerger {}.StartAndEnd("X", "Y");
            CHECK(m.Generate(empty) == "XY");
            CHECK(m.Generate(ty) == "XtyY");
            CHECK(m.Generate(abc) == "XabcY");
        }

        SECTION("set start & end to same")
        {
            const auto m = euco::StringMerger {}.StartAndEnd("X");
            CHECK(m.Generate(empty) == "XX");
            CHECK(m.Generate(ty) == "XtyX");
            CHECK(m.Generate(abc) == "XabcX");
        }

        SECTION("set sep")
        {
            const auto m = euco::StringMerger {}.Separator("X");
            CHECK(m.Generate(empty).empty());
            CHECK(m.Generate(ty) == "tXy");
            CHECK(m.Generate(abc) == "aXbXc");
        }

        SECTION("set sep and final")
        {
            const auto m = euco::StringMerger {}.Separator("X", "Y");
            CHECK(m.Generate(empty).empty());
            CHECK(m.Generate(ty) == "tYy");
            CHECK(m.Generate(abc) == "aXbYc");
        }

        SECTION("set empty")
        {
            const auto m = euco::StringMerger {}.Empty("X");
            CHECK(m.Generate(empty) == "X");
            CHECK(m.Generate(ty) == "ty");
            CHECK(m.Generate(abc) == "abc");
        }

        SECTION("before each")
        {
            const auto m = euco::StringMerger {}.BeforeEach("X");
            CHECK(m.Generate(empty).empty());
            CHECK(m.Generate(dog) == "Xdog");
            CHECK(m.Generate(ty) == "XtXy");
            CHECK(m.Generate(abc) == "XaXbXc");
        }

        SECTION("after each")
        {
            const auto m = euco::StringMerger {}.AfterEach("X");
            CHECK(m.Generate(empty).empty());
            CHECK(m.Generate(dog) == "dogX");
            CHECK(m.Generate(ty) == "tXyX");
            CHECK(m.Generate(abc) == "aXbXcX");
        }

        SECTION("after each and final")
        {
            const auto m = euco::StringMerger {}.AfterEach("X", "Y");
            CHECK(m.Generate(empty).empty());
            CHECK(m.Generate(dog) == "dogY");
            CHECK(m.Generate(ty) == "tXyY");
            CHECK(m.Generate(abc) == "aXbXcY");
        }
    }
}
