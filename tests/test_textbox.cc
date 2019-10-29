#include "core/textbox.h"
#include "catch.hpp"

#include <vector>
#include <string>

using Catch::Matchers::Equals;

constexpr bool PRINT_HEX = false;

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

            if constexpr (PRINT_HEX)
            {
                s << "(";
                bool f = true;
                for(auto c: st)
                {
                    if(f) f=false;
                    else s << " ";
                    s << static_cast<int>(c);
                }
                s << ")";
            }
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

    std::string ascii(std::initializer_list<int> ii)
    {
        std::string r;
        for(auto i: ii)
        {
            r += static_cast<char>(i);
        }
        return r;
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
        CHECK_THAT(box.to_string(), Eq({}));
    }

    SECTION("putchar")
    {
        SECTION("0")
        {
            box.putchar('x', 0, 0);

            INFO(box.data);
            CHECK(box.Size() == S(1,1));
            CHECK_THAT(box.data, Eq({"x"}));
            CHECK_THAT(box.to_string(), Eq({"x"}));
        }

        SECTION("offset")
        {
            box.putchar('x', 2, 1);

            INFO(box.data);
            CHECK(box.Size() == S(3,2));
            CHECK_THAT(box.data, Eq({"", "  x"}));
            CHECK_THAT(box.to_string(), Eq({"  x"}));
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
            CHECK_THAT(box.to_string(), Eq({"d"}));
        }

        SECTION("offset")
        {
            box.modchar(2, 1, [](char& c){c = 'd';});

            INFO(box.data);
            CHECK(box.Size() == S(3,2));
            CHECK_THAT(box.data, Eq({"", "  d"}));
            CHECK_THAT(box.to_string(), Eq({"  d"}));
        }
    }

    SECTION("putline")
    {
        SECTION("0")
        {
            box.putline("dog", 0, 0);

            INFO(box.data);
            CHECK(box.Size() == S(3,1));
            CHECK_THAT(box.data, Eq({"dog"}));
            CHECK_THAT(box.to_string(), Eq({"dog"}));
        }

        SECTION("offset")
        {
            box.putline("dog", 1, 1);

            INFO(box.data);
            CHECK(box.Size() == S(4,2));
            CHECK_THAT(box.data, Eq({"", " dog"}));
            CHECK_THAT(box.to_string(), Eq({" dog"}));
        }

        SECTION("collision")
        {
            box.putline("doggo", 0, 0);
            box.putline( "dog", 1, 0);

            INFO(box.data);
            CHECK(box.Size() == S(5,1));
            CHECK_THAT(box.data, Eq({"ddogo"}));
            CHECK_THAT(box.to_string(), Eq({"ddogo"}));
        }
    }

    // putbox

    SECTION("trim")
    {
        SECTION("empty")
        {
            box.trim();

            INFO(box.data);
            CHECK(box.Size() == S(0,0));
            CHECK_THAT(box.data, Eq({}));
            CHECK_THAT(box.to_string(), Eq({}));
        }

        SECTION("no change")
        {
            box.data = {"", "  a"};
            box.trim();

            INFO(box.data);
            CHECK(box.Size() == S(3,2));
            CHECK_THAT(box.data, Eq({"", "  a"}));
            CHECK_THAT(box.to_string(), Eq({"  a"}));
        }

        SECTION("change")
        {
            box.data = {"    ", "  a ", ""};
            box.trim();

            INFO(box.data);
            CHECK(box.Size() == S(3,2));
            CHECK_THAT(box.data, Eq({"", "  a"}));
            CHECK_THAT(box.to_string(), Eq({"  a"}));
        }
    }

    // todo(Gustav): fix bitmasks in character codes
    SECTION("lines")
    {
        SECTION("h1")
        {
            box.hline(0, 0, 3, false, false);

            INFO(box.data);
            CHECK(box.Size() == S(3,1));
            CHECK_THAT(box.data, Eq({ascii({8, 12, 4})}));
            CHECK_THAT(box.to_string(), Eq({"---"}));
        }

        SECTION("h2")
        {
            box.hline(0, 0, 3, false, true);

            INFO(box.data);
            CHECK(box.Size() == S(3,1));
            CHECK_THAT(box.data, Eq({ascii({8, 12, 12})}));
            CHECK_THAT(box.to_string(), Eq({"---"}));
        }

        SECTION("h3")
        {
            box.hline(0, 0, 3, true, false);

            INFO(box.data);
            CHECK(box.Size() == S(3,1));
            CHECK_THAT(box.data, Eq({ascii({12, 12, 4})}));
            CHECK_THAT(box.to_string(), Eq({"---"}));
        }

        SECTION("h4")
        {
            box.hline(0, 0, 3, true, true);

            INFO(box.data);
            CHECK(box.Size() == S(3,1));
            CHECK_THAT(box.data, Eq({ascii({12, 12, 12})}));
            CHECK_THAT(box.to_string(), Eq({"---"}));
        }

        SECTION("v1")
        {
            box.vline(0, 0, 3, false, false);

            INFO(box.data);
            CHECK(box.Size() == S(1,3));
            CHECK_THAT(box.data, Eq({ascii({2}), ascii({3}), ascii({1})}));
            CHECK_THAT(box.to_string(), Eq({"|", "|", "|"}));
        }

        SECTION("v2")
        {
            box.vline(0, 0, 3, false, true);

            INFO(box.data);
            CHECK(box.Size() == S(1,3));
            CHECK_THAT(box.data, Eq({ascii({2}), ascii({3}), ascii({3})}));
            CHECK_THAT(box.to_string(), Eq({"|", "|", "|"}));
        }

        SECTION("v3")
        {
            box.vline(0, 0, 3, true, false);

            INFO(box.data);
            CHECK(box.Size() == S(1,3));
            CHECK_THAT(box.data, Eq({ascii({3}), ascii({3}), ascii({1})}));
            CHECK_THAT(box.to_string(), Eq({"|", "|", "|"}));
        }

        SECTION("v4")
        {
            box.vline(0, 0, 3, true, true);

            INFO(box.data);
            CHECK(box.Size() == S(1,3));
            CHECK_THAT(box.data, Eq({ascii({3}), ascii({3}), ascii({3})}));
            CHECK_THAT(box.to_string(), Eq({"|", "|", "|"}));
        }
    }
}
