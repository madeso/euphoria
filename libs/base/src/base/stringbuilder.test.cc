#include "base/stringbuilder.h"

#include "catch.hpp"

using namespace eu;


TEST_CASE("stringbuilder", "[stringbuilder]")
{
    SECTION("empty")
    {
        StringBuilder ss;
        REQUIRE(ss.to_string().empty());
    }

    SECTION("chars")
    {
        auto ss = StringBuilder{};
        ss.add_char('d');
        ss.add_char('o');
        ss.add_char('g');
        REQUIRE(ss.to_string() == "dog");
    }

    SECTION("strings")
    {
        auto ss = StringBuilder{};
        ss.add_string("dog");
        ss.add_string(" is ");
        ss.add_string("good");
        REQUIRE(ss.to_string() == "dog is good");
    }

    SECTION("string view")
    {
        auto ss = StringBuilder{};
        ss.add_view("good ");
        ss.add_view("dog!");
        REQUIRE(ss.to_string() == "good dog!");
    }
}
