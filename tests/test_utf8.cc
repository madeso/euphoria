#include "core/utf8.h"
#include "catch.hpp"
#include <vector>
#include <string>

#include "tests/stringeq.h"

using namespace euphoria::core;

namespace
{
    using T = std::vector<int>;

    std::pair<bool, T>
    Parse(const std::string& str)
    {
        auto ret = T{};
        
        const auto r = Utf8ToCodepoints(str,
            [&](const int cp) { ret.push_back(cp); }
        );

        return std::make_pair(r, ret);
    }
}

TEST_CASE("utf8_tests")
{
    SECTION("empty")
    {
        const auto [ok, code_points] = Parse(u8"");
        CHECK(ok);
        CHECK_THAT(code_points, Catch::Equals(T{}));
    }
    SECTION("Dollar sign")
    {
        const auto [ok, code_points] = Parse(u8"$");
        CHECK(ok);
        CHECK_THAT(code_points, Catch::Equals(T{044}));
    }
    SECTION("Cent sign")
    {
        const auto [ok, code_points] = Parse(u8"¢");
        CHECK(ok);
        CHECK_THAT(code_points, Catch::Equals(T{0242}));
    }
    SECTION("Devanagari character")
    {
        const auto [ok, code_points] = Parse(u8"ह");
        CHECK(ok);
        CHECK_THAT(code_points, Catch::Equals(T{004471}));
    }
    SECTION("Euro sign")
    {
        const auto [ok, code_points] = Parse(u8"€");
        CHECK(ok);
        CHECK_THAT(code_points, Catch::Equals(T{020254}));
    }
    SECTION("Hwair")
    {
        const auto [ok, code_points] = Parse(u8"𐍈");
        CHECK(ok);
        CHECK_THAT(code_points, Catch::Equals(T{0201510}));
    }
}
