#include "core/textbox.h"
#include "catch.hpp"

#include <vector>
#include <string>

#include "tests/stringeq.h"

constexpr bool PRINT_HEX = false;

using namespace euphoria::core;
using namespace euphoria::tests;

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

TEST_CASE("tb_print")
{
    auto box = TextBox::Empty();

    SECTION("empty")
    {
        INFO(box.data);
        CHECK(box.Size() == S(0,0));
        CHECK(StringEq(box.data, {}));
        CHECK(StringEq(box.to_string(AsciiStyle()), {}));
    }

    SECTION("putchar")
    {
        SECTION("0")
        {
            box.putchar('x', 0, 0);

            INFO(box.data);
            CHECK(box.Size() == S(1,1));
            CHECK(StringEq(box.data, {"x"}));
            CHECK(StringEq(box.to_string(AsciiStyle()), {"x"}));
        }

        SECTION("offset")
        {
            box.putchar('x', 2, 1);

            INFO(box.data);
            CHECK(box.Size() == S(3,2));
            CHECK(StringEq(box.data, {"", "  x"}));
            CHECK(StringEq(box.to_string(AsciiStyle()), {"", "  x"}));
        }
    }

    SECTION("modchar")
    {
        SECTION("0")
        {
            box.modchar(0, 0, [](char& c){c = 'd';});

            INFO(box.data);
            CHECK(box.Size() == S(1,1));
            CHECK(StringEq(box.data, {"d"}));
            CHECK(StringEq(box.to_string(AsciiStyle()), {"d"}));
        }

        SECTION("offset")
        {
            box.modchar(2, 1, [](char& c){c = 'd';});

            INFO(box.data);
            CHECK(box.Size() == S(3,2));
            CHECK(StringEq(box.data, {"", "  d"}));
            CHECK(StringEq(box.to_string(AsciiStyle()), {"", "  d"}));
        }
    }

    SECTION("putline")
    {
        SECTION("0")
        {
            box.putline("dog", 0, 0);

            INFO(box.data);
            CHECK(box.Size() == S(3,1));
            CHECK(StringEq(box.data, {"dog"}));
            CHECK(StringEq(box.to_string(AsciiStyle()), {"dog"}));
        }

        SECTION("offset")
        {
            box.putline("dog", 1, 1);

            INFO(box.data);
            CHECK(box.Size() == S(4,2));
            CHECK(StringEq(box.data, {"", " dog"}));
            CHECK(StringEq(box.to_string(AsciiStyle()), {"", " dog"}));
        }

        SECTION("collision")
        {
            box.putline("doggo", 0, 0);
            box.putline( "dog", 1, 0);

            INFO(box.data);
            CHECK(box.Size() == S(5,1));
            CHECK(StringEq(box.data, {"ddogo"}));
            CHECK(StringEq(box.to_string(AsciiStyle()), {"ddogo"}));
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
            CHECK(StringEq(box.data, {}));
            CHECK(StringEq(box.to_string(AsciiStyle()), {}));
        }

        SECTION("no change")
        {
            box.data = {"", "  a"};
            box.trim();

            INFO(box.data);
            CHECK(box.Size() == S(3,2));
            CHECK(StringEq(box.data, {"", "  a"}));
            CHECK(StringEq(box.to_string(AsciiStyle()), {"", "  a"}));
        }

        SECTION("change")
        {
            box.data = {"    ", "  a ", ""};
            box.trim();

            INFO(box.data);
            CHECK(box.Size() == S(3,2));
            CHECK(StringEq(box.data, {"", "  a"}));
            CHECK(StringEq(box.to_string(AsciiStyle()), {"", "  a"}));
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
            CHECK(StringEq(box.data, {ascii({8, 12, 4})}));
            CHECK(StringEq(box.to_string(AsciiStyle()), {"---"}));
        }

        SECTION("h2")
        {
            box.hline(0, 0, 3, false, true);

            INFO(box.data);
            CHECK(box.Size() == S(3,1));
            CHECK(StringEq(box.data, {ascii({8, 12, 12})}));
            CHECK(StringEq(box.to_string(AsciiStyle()), {"---"}));
        }

        SECTION("h3")
        {
            box.hline(0, 0, 3, true, false);

            INFO(box.data);
            CHECK(box.Size() == S(3,1));
            CHECK(StringEq(box.data, {ascii({12, 12, 4})}));
            CHECK(StringEq(box.to_string(AsciiStyle()), {"---"}));
        }

        SECTION("h4")
        {
            box.hline(0, 0, 3, true, true);

            INFO(box.data);
            CHECK(box.Size() == S(3,1));
            CHECK(StringEq(box.data, {ascii({12, 12, 12})}));
            CHECK(StringEq(box.to_string(AsciiStyle()), {"---"}));
        }

        SECTION("v1")
        {
            box.vline(0, 0, 3, false, false);

            INFO(box.data);
            CHECK(box.Size() == S(1,3));
            CHECK(StringEq(box.data, {ascii({2}), ascii({3}), ascii({1})}));
            CHECK(StringEq(box.to_string(AsciiStyle()), {"|", "|", "|"}));
        }

        SECTION("v2")
        {
            box.vline(0, 0, 3, false, true);

            INFO(box.data);
            CHECK(box.Size() == S(1,3));
            CHECK(StringEq(box.data, {ascii({2}), ascii({3}), ascii({3})}));
            CHECK(StringEq(box.to_string(AsciiStyle()), {"|", "|", "|"}));
        }

        SECTION("v3")
        {
            box.vline(0, 0, 3, true, false);

            INFO(box.data);
            CHECK(box.Size() == S(1,3));
            CHECK(StringEq(box.data, {ascii({3}), ascii({3}), ascii({1})}));
            CHECK(StringEq(box.to_string(AsciiStyle()), {"|", "|", "|"}));
        }

        SECTION("v4")
        {
            box.vline(0, 0, 3, true, true);

            INFO(box.data);
            CHECK(box.Size() == S(1,3));
            CHECK(StringEq(box.data, {ascii({3}), ascii({3}), ascii({3})}));
            CHECK(StringEq(box.to_string(AsciiStyle()), {"|", "|", "|"}));
        }
    }
}

// todo(Gustav): How is this supposed to work?
TEST_CASE("tb_box")
{
    const std::vector<std::string> x_data = {"   ", " x ", "   "};
    const std::vector<std::string> abc_data = {"abc"};

    const auto empty = TextBox::Empty();
    const auto x = TextBox::FromString(x_data);
    const auto abc = TextBox::FromString(abc_data);

    SECTION("putbox")
    {
        CHECK(StringEq(empty.PutBoxCopy(0, 0, empty).to_string(AsciiStyle()), {}));
        CHECK(StringEq(empty.PutBoxCopy(0, 0, x).to_string(AsciiStyle()), x_data));
        CHECK(StringEq(empty.PutBoxCopy(0, 0, abc).to_string(AsciiStyle()), abc_data));
        CHECK(StringEq(empty.PutBoxCopy(1, 1, empty).to_string(AsciiStyle()), {}));
        CHECK(StringEq(empty.PutBoxCopy(1, 1, x).to_string(AsciiStyle()), {"", "    ", "  x ", "    "}));
        CHECK(StringEq(empty.PutBoxCopy(1, 1, empty).to_string(AsciiStyle()), {}));
    }

    SECTION("empty")
    {
        CHECK(empty.horiz_append_position(0, empty) == 0);
        CHECK(empty.vert_append_position(0, empty) == 0);

        CHECK(empty.horiz_append_position(0, x) == 0);
        CHECK(empty.vert_append_position(0, x) == 0);

        CHECK(empty.horiz_append_position(0, abc) == 0);
        CHECK(empty.vert_append_position(0, abc) == 0);

        CHECK(empty.horiz_append_position(1, empty) == 0);
        CHECK(empty.vert_append_position(1, empty) == 0);

        CHECK(empty.horiz_append_position(1, x) == 0);
        CHECK(empty.vert_append_position(1, x) == 0);

        CHECK(empty.horiz_append_position(1, abc) == 0);
        CHECK(empty.vert_append_position(1, abc) == 0);
    }

    SECTION("x")
    {
        CHECK(x.horiz_append_position(0, empty) == 0);
        CHECK(x.vert_append_position(0, empty) == 0);

        CHECK(x.horiz_append_position(0, x) == 1);
        CHECK(x.vert_append_position(0, x) == 1);
        
        CHECK(x.horiz_append_position(0, abc) == 0);
        CHECK(x.vert_append_position(0, abc) == 2);
        
        CHECK(x.horiz_append_position(1, empty) == 0);
        CHECK(x.vert_append_position(1, empty) == 0);
        
        CHECK(x.horiz_append_position(1, x) == 0);
        CHECK(x.vert_append_position(1, x) == 0);
        
        CHECK(x.horiz_append_position(1, abc) == 2);
        CHECK(x.vert_append_position(1, abc) == 2);
    }

    SECTION("abc")
    {
        CHECK(abc.horiz_append_position(0, empty) == 0);
        CHECK(abc.vert_append_position(0, empty) == 0);
        
        CHECK(abc.horiz_append_position(0, x) == 0);
        CHECK(abc.vert_append_position(0, x) == 0);
        
        CHECK(abc.horiz_append_position(0, abc) == 3);
        CHECK(abc.vert_append_position(0, abc) == 1);
        
        CHECK(abc.horiz_append_position(1, empty) == 0);
        CHECK(abc.vert_append_position(1, empty) == 0);
        
        CHECK(abc.horiz_append_position(1, x) == 0);
        CHECK(abc.vert_append_position(1, x) == 0);
        
        CHECK(abc.horiz_append_position(1, abc) == 0);
        CHECK(abc.vert_append_position(1, abc) == 1);
    }
}

TEST_CASE("tb_arrows")
{
    auto box = TextBox::Empty();
    for(int i=1; i<16; i+=1)
    {
        box.putchar(static_cast<char>(i), (i % 4), (i/4));
    }

    CHECK(StringEq(box.to_string(AsciiStyle()),
    {
        " |||",
        "-'.+",
        "-`,+",
        "-+++"
    }));
}

namespace
{
    struct T
    {
        std::string name;
        std::vector<T> children;

        T(const std::string& n) : name(n) {}
        T(const std::string& n, std::initializer_list<T> t) : name(n), children(t) {}
    };
}

TEST_CASE("tb_create_tree_graph")
{
    const auto simple_tree = T("a", { T{"1"}, T{"2"}, T{"3"} });

    const auto simple_two_row = std::vector<std::string>
        {
            "a--+--+--.",
            "   1  2  3"
        };
    const auto simple_three_row = std::vector<std::string>
        {
            "a",
            "`-+----+----.",
            "  1    2    3"
        };
    const auto simple_four_row = std::vector<std::string>
        {
            "a",
            "+-1",
            "`-+----.",
            "  2    3"
        };

    CHECK(StringEq(create_tree_graph(simple_tree, 130,
                [](const T& e) { return e.name; },
                [](const T& e) { return std::make_pair(e.children.cbegin(), e.children.cend()); },
                [](const T&  ) { return false; },
                [](const T&  ) { return false; },
                [](const T&  ) { return false; }).to_string(AsciiStyle()),
                simple_three_row));

    CHECK(StringEq(create_tree_graph(simple_tree, 130,
                [](const T& e) { return e.name; },
                [](const T& e) { return std::make_pair(e.children.cbegin(), e.children.cend()); },
                [](const T&  ) { return false; },
                [](const T&  ) { return false; },
                [](const T&  ) { return true; }).to_string(AsciiStyle()),
                simple_four_row));
    
    CHECK(StringEq(create_tree_graph(simple_tree, 130,
                [](const T& e) { return e.name; },
                [](const T& e) { return std::make_pair(e.children.cbegin(), e.children.cend()); },
                [](const T&  ) { return false; },
                [](const T&  ) { return true; },
                [](const T&  ) { return false; }).to_string(AsciiStyle()),
                simple_three_row));

    CHECK(StringEq(create_tree_graph(simple_tree, 130,
                [](const T& e) { return e.name; },
                [](const T& e) { return std::make_pair(e.children.cbegin(), e.children.cend()); },
                [](const T&  ) { return false; },
                [](const T&  ) { return true; },
                [](const T&  ) { return true; }).to_string(AsciiStyle()),
                simple_four_row));


    CHECK(StringEq(create_tree_graph(simple_tree, 130,
                [](const T& e) { return e.name; },
                [](const T& e) { return std::make_pair(e.children.cbegin(), e.children.cend()); },
                [](const T&  ) { return true; },
                [](const T&  ) { return false; },
                [](const T&  ) { return false; }).to_string(AsciiStyle()),
                simple_two_row));

    CHECK(StringEq(create_tree_graph(simple_tree, 130,
                [](const T& e) { return e.name; },
                [](const T& e) { return std::make_pair(e.children.cbegin(), e.children.cend()); },
                [](const T&  ) { return true; },
                [](const T&  ) { return false; },
                [](const T&  ) { return true; }).to_string(AsciiStyle()),
                simple_four_row));
    
    CHECK(StringEq(create_tree_graph(simple_tree, 130,
                [](const T& e) { return e.name; },
                [](const T& e) { return std::make_pair(e.children.cbegin(), e.children.cend()); },
                [](const T&  ) { return true; },
                [](const T&  ) { return true; },
                [](const T&  ) { return false; }).to_string(AsciiStyle()),
                simple_two_row));

    CHECK(StringEq(create_tree_graph(simple_tree, 130,
                [](const T& e) { return e.name; },
                [](const T& e) { return std::make_pair(e.children.cbegin(), e.children.cend()); },
                [](const T&  ) { return true; },
                [](const T&  ) { return true; },
                [](const T&  ) { return true; }).to_string(AsciiStyle()),
                simple_four_row));
}


TEST_CASE("tb_tolkien")
{
    const auto tolkien_tree =
        T{"Tolkien characters", {
            T{"Heroes", {
                T{"Humans", {
                    T{"Aragon"},
                    T{"Boromir"}
                }},
                T{"Hobbits", {
                    T{"Frodo"},
                    T{"Samwise"},
                    T{"Merry"},
                    T{"Pippin"},
                }},
                T{"Other", {
                    T{"Legolas"},
                    T{"Gandalf"},
                    T{"Gimli"}
                }}
            }},
            T{"Enemies", {
                T{"Sauron"},
                T{"orcs"},
                T{"Sauruman"}
            }}
        }};
    CHECK(StringEq(create_tree_graph(tolkien_tree, 130,
                [](const T& e) { return e.name; },
                [](const T& e) { return std::make_pair(e.children.cbegin(), e.children.cend()); },
                [](const T& e) { return e.children.size() >= 1; },
                [](const T&  ) { return true; },
                [](const T&  ) { return false; }).to_string(AsciiStyle()),
                {
  "Tolkien characters",
  "`-+----------------------------------------------------------------------------------.",
  "  Heroes--+------------------+----------------------------------.                    Enemies--+-------+-----.",
  "          Humans--+-------.  Hobbits--+------+--------+------.  Other--+--------+--------.    Sauron  orcs  Sauruman",
  "                  Aragon  Boromir     Frodo  Samwise  Merry  Pippin    Legolas  Gandalf  Gimli"
                }));
}
