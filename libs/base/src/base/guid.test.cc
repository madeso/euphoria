#include "base/guid.h"

#include "catch2/catch_all.hpp"

using namespace eu;


TEST_CASE("guid-test", "[guid]")
{
    SECTION("guid ctor")
    {
        CHECK(string_from(Guid{0x12344567, 0xabcd, 0x1234, 0xabba, 0x123456789abc}) == "12344567-abcd-1234-abba-123456789abc");
        CHECK(string_from(Guid{0xffffffff, 0xffff, 0xffff, 0xffff, 0xffffffffffff}) == "ffffffff-ffff-ffff-ffff-ffffffffffff");
    }
    
    SECTION("nil")
    {
        CHECK(string_from(nil_guid) == "00000000-0000-0000-0000-000000000000");
        CHECK(nil_guid == Guid{0x00000000, 0x0000, 0x0000, 0x0000, 0x000000000000});
    }

    SECTION("generate does not return nill")
    {
        const auto g = Guid::generate();
        REQUIRE(g);
        CHECK(*g != nil_guid);
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
        const auto parsed = guid_from(src);
        INFO(src);
        REQUIRE(parsed.has_value());
        CHECK(string_from(*parsed) == "123e4567-e89b-12d3-a456-426614174000");
        CHECK(*parsed == Guid{0x123e4567, 0xe89b, 0x12d3, 0xa456, 0x426614174000});
    }

    SECTION("parse fail")
    {
        const std::string src = GENERATE
        (
            "not a guid",
            "123e4567-e89b-12d3-a456-4266141740" // last 2 zeroes removed
        );
        const auto parsed = guid_from(src);
        CHECK_FALSE(parsed.has_value());
    }
}

TEST_CASE("guid-print", "[guid]")
{
    const auto g = Guid{0x123e4567, 0xe89b, 0x12d3, 0xa456, 0x426614174000};

    // todo(Gustav): how reliable are theese tests, switch to regex?
    SECTION("fmt")
    {
        std::string on = fmt::format("{0}", nil_guid);
        std::string og = fmt::format("{0}", g);
        CHECK(on == "00000000-0000-0000-0000-000000000000");
        CHECK(og == "123e4567-e89b-12d3-a456-426614174000");
    }

    SECTION("stream")
    {
        std::ostringstream on; on << nil_guid;
        std::ostringstream og; og << g;
        CHECK(on.str() == "00000000-0000-0000-0000-000000000000");
        CHECK(og.str() == "123e4567-e89b-12d3-a456-426614174000");
    }
}

