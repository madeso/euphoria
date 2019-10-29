#include "core/textbox.h"
#include "catch.hpp"

#include <vector>
#include <string>

using Catch::Matchers::Equals;

namespace std
{
    std::ostream& operator<<(std::ostream& s, const std::vector<std::string>& v)
    {
        s << "{";
        bool first = true;
        for(auto st: v)
        {
            if(first) first = false;
            else s << ", ";
            s << '"' << st << '"';
        }
        s << "}";
        return s;
    }

    std::ostream& operator<<(std::ostream& s, const std::pair<size_t, size_t>& t)
    {
        s << "(" << t.first << ", " << t.second << ")";
        return s;
    }
}

namespace
{
    std::pair<size_t, size_t> S(size_t a, size_t b)
    {
        return {a, b};
    }
}

using Eq = Catch::Vector::EqualsMatcher<std::string>;

ANON_TEST_CASE()
{
    TextBox box;

    SECTION("empty")
    {
        INFO(box.data);
        CHECK(box.Size() == S(0,0));
        CHECK_THAT(box.data, Eq({}));
    }

    SECTION("putchar")
    {
        SECTION("0")
        {
            box.putchar('x', 0, 0);

            INFO(box.data);
            CHECK(box.Size() == S(1,1));
            CHECK_THAT(box.data, Eq({"x"}));
        }

        SECTION("offset")
        {
            box.putchar('x', 2, 1);

            INFO(box.data);
            CHECK(box.Size() == S(3,2));
            CHECK_THAT(box.data, Eq({"", "  x"}));
        }
    }

    SECTION("modchar")
    {
        SECTION("0")
        {
            box.modchar(0, 0, [](char& c){c = 'd';});

            INFO(box.data);
            CHECK(box.Size() == S(1,1));
            CHECK_THAT(box.data, Eq({"d"}));
        }

        SECTION("offset")
        {
            box.modchar(2, 1, [](char& c){c = 'd';});

            INFO(box.data);
            CHECK(box.Size() == S(3,2));
            CHECK_THAT(box.data, Eq({"", "  d"}));
        }
    }

    SECTION("trim")
    {
        SECTION("empty")
        {
            box.trim();

            INFO(box.data);
            CHECK(box.Size() == S(0,0));
            CHECK_THAT(box.data, Eq({}));
        }

        SECTION("no change")
        {
            box.data = {"", "  a"};
            box.trim();

            INFO(box.data);
            CHECK(box.Size() == S(3,2));
            CHECK_THAT(box.data, Eq({"", "  a"}));
        }

        SECTION("change")
        {
            box.data = {"    ", "  a ", ""};
            box.trim();

            INFO(box.data);
            CHECK(box.Size() == S(3,2));
            CHECK_THAT(box.data, Eq({"", "  a"}));
        }
    }
}
