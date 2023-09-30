#include "core/console.h"

#include "catch.hpp"

namespace core = eu::core;

TEST_CASE("commandline", "[commandline]")
{
    SECTION("empty cmd")
    {
        const auto empty = std::vector<std::string>{};
        CHECK(empty == core::parse_commandline(""));
    }

    SECTION("single cmd")
    {
        CHECK(std::vector<std::string> {"dog"}
              == core::parse_commandline("dog"));
    }

    SECTION("arguments")
    {
        const auto vec = std::vector<std::string> {"dog", "cat", "cat"};
        CHECK(vec == core::parse_commandline("dog cat cat"));
    }

    SECTION("with backslash")
    {
        const auto vec = std::vector<std::string> {"dog", "cat\\dog"};
        CHECK(vec == core::parse_commandline("dog cat\\\\dog"));
    }

    SECTION("several spaces")
    {
        const auto vec = std::vector<std::string> {"dog", "cat", "cat"};
        CHECK(vec == core::parse_commandline("   dog    cat   cat   "));
    }

    SECTION("todo different types of spaces")
    {
        const auto vec = std::vector<std::string> {"dog", "cat", "cat"};
        CHECK(vec == core::parse_commandline("\tdog\tcat\tcat\t"));
    }

    SECTION("escapes")
    {
        const auto vec = std::vector<std::string> {"dog", "\n", "\t"};
        CHECK(vec == core::parse_commandline("dog \\n \\t"));
    }

    SECTION("escapes without space")
    {
        const auto vec = std::vector<std::string> {"dog", "cat", "cat"};
        CHECK(vec == core::parse_commandline("dog cat \"cat\""));
    }

    SECTION("escapes with space")
    {
        const auto vec = std::vector<std::string> {"dog", "cat", "cat and dog"};
        CHECK(vec == core::parse_commandline("dog cat \"cat and dog\""));
    }

    SECTION("escapes with missing quote")
    {
        const auto vec = std::vector<std::string> {"dog", "cat", "cat and dog"};
        CHECK(vec == core::parse_commandline("dog cat \"cat and dog"));
    }

    SECTION("escapes with quotes")
    {
        const auto vec
                = std::vector<std::string> {"dog", "cat", "cat and \"dog\""};
        CHECK(vec == core::parse_commandline("dog cat \"cat and \\\"dog\\\"\""));
    }

    SECTION("escapes without quotes")
    {
        const auto vec = std::vector<std::string> {"dog", "\"cat\""};
        CHECK(vec == core::parse_commandline("dog \\\"cat\\\""));
    }

    SECTION("end with escape")
    {
        const auto vec = std::vector<std::string> {"dog", "\"cat"};
        CHECK(vec == core::parse_commandline("dog \\\"cat\\"));
    }

    SECTION("string end with escape")
    {
        const auto vec = std::vector<std::string> {"dog", "cat"};
        CHECK(vec == core::parse_commandline("dog \"cat\\"));
    }

    SECTION("single quote strings")
    {
        const auto vec = std::vector<std::string> {"dog", "cat and dog"};
        CHECK(vec == core::parse_commandline("dog 'cat and dog'"));
    }

    SECTION("single quote in double quote")
    {
        const auto vec = std::vector<std::string> {"dog", "'cat'"};
        CHECK(vec == core::parse_commandline("dog \"'cat'\""));
    }

    SECTION("double quote in single quote")
    {
        const auto vec = std::vector<std::string> {"dog", "\"cat\""};
        CHECK(vec == core::parse_commandline("dog '\"cat\"'"));
    }
}
