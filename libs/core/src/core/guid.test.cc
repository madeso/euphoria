#include "core/guid.h"

#include "catch2/catch_all.hpp"

using namespace eu::core;

TEST_CASE("guid-test", "[guid]")
{
    SECTION("nil")
    {
        CHECK(nil_guid.to_string() == "00000000-0000-0000-0000-000000000000");
    }

    SECTION("parse ok")
    {
        const std::string src = GENERATE
        (
            "123e4567-e89b-12d3-a456-426614174000",
            "    123e4567-e89b-12d3-a456-426614174000",
            "{123e4567-e89b-12d3-a456-426614174000}",
            "123E4567-E89B-12D3-A456-426614174000",
            "{123E4567-E89B-12D3-A456-426614174000}",
            "12 3e 45 67 - e8 9b - 12 d3 - a4 56 - 42 66 14 17 40 00 junk data here"
        );
        const auto parsed = Guid::parse(src);
        REQUIRE(parsed.has_value());
        CHECK(parsed->to_string() == "123e4567-e89b-12d3-a456-426614174000");
    }

    SECTION("parse ok")
    {
        const std::string src = GENERATE
        (
            "not a guid",
            "123e4567-e89b-12d3-a456-4266141740" // last 2 zeroes removed
        );
        const auto parsed = Guid::parse(src);
        CHECK_FALSE(parsed.has_value());
    }
}
