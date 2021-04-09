#include "core/console.h"

#include "catch.hpp"

namespace euco = euphoria::core;

TEST_CASE("commandline", "[commandline]")
{
    SECTION("empty cmd")
    {
        const auto empty = std::vector<std::string>{};
        CHECK(empty == euco::ParseCommandLine(""));
    }

    SECTION("single cmd")
    {
        CHECK(std::vector<std::string> {"dog"}
              == euco::ParseCommandLine("dog"));
    }

    SECTION("arguments")
    {
        const auto vec = std::vector<std::string> {"dog", "cat", "cat"};
        CHECK(vec == euco::ParseCommandLine("dog cat cat"));
    }

    SECTION("with backslash")
    {
        const auto vec = std::vector<std::string> {"dog", "cat\\dog"};
        CHECK(vec == euco::ParseCommandLine("dog cat\\\\dog"));
    }

    SECTION("several spaces")
    {
        const auto vec = std::vector<std::string> {"dog", "cat", "cat"};
        CHECK(vec == euco::ParseCommandLine("   dog    cat   cat   "));
    }

    SECTION("todo different types of spaces")
    {
        const auto vec = std::vector<std::string> {"dog", "cat", "cat"};
        CHECK(vec == euco::ParseCommandLine("\tdog\tcat\tcat\t"));
    }

    SECTION("escapes")
    {
        const auto vec = std::vector<std::string> {"dog", "\n", "\t"};
        CHECK(vec == euco::ParseCommandLine("dog \\n \\t"));
    }

    SECTION("escapes without space")
    {
        const auto vec = std::vector<std::string> {"dog", "cat", "cat"};
        CHECK(vec == euco::ParseCommandLine("dog cat \"cat\""));
    }

    SECTION("escapes with space")
    {
        const auto vec = std::vector<std::string> {"dog", "cat", "cat and dog"};
        CHECK(vec == euco::ParseCommandLine("dog cat \"cat and dog\""));
    }

    SECTION("escapes with missing quote")
    {
        const auto vec = std::vector<std::string> {"dog", "cat", "cat and dog"};
        CHECK(vec == euco::ParseCommandLine("dog cat \"cat and dog"));
    }

    SECTION("escapes with quotes")
    {
        const auto vec
                = std::vector<std::string> {"dog", "cat", "cat and \"dog\""};
        CHECK(vec == euco::ParseCommandLine("dog cat \"cat and \\\"dog\\\"\""));
    }

    SECTION("escapes without quotes")
    {
        const auto vec = std::vector<std::string> {"dog", "\"cat\""};
        CHECK(vec == euco::ParseCommandLine("dog \\\"cat\\\""));
    }

    SECTION("end with escape")
    {
        const auto vec = std::vector<std::string> {"dog", "\"cat"};
        CHECK(vec == euco::ParseCommandLine("dog \\\"cat\\"));
    }

    SECTION("string end with escape")
    {
        const auto vec = std::vector<std::string> {"dog", "cat"};
        CHECK(vec == euco::ParseCommandLine("dog \"cat\\"));
    }

    SECTION("single quote strings")
    {
        const auto vec = std::vector<std::string> {"dog", "cat and dog"};
        CHECK(vec == euco::ParseCommandLine("dog 'cat and dog'"));
    }

    SECTION("single quote in double quote")
    {
        const auto vec = std::vector<std::string> {"dog", "'cat'"};
        CHECK(vec == euco::ParseCommandLine("dog \"'cat'\""));
    }

    SECTION("double quote in single quote")
    {
        const auto vec = std::vector<std::string> {"dog", "\"cat\""};
        CHECK(vec == euco::ParseCommandLine("dog '\"cat\"'"));
    }
}
