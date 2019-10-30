#include "core/textbox.h"
#include "catch.hpp"

#include <vector>
#include <string>

#include "tests/stringeq.h"

using Catch::Matchers::Equals;

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

using Eq = Catch::Vector::EqualsMatcher<std::string>;

TEST_CASE("tb_print")
{
    TextBox box;

    SECTION("empty")
    {
        INFO(box.data);
        CHECK(box.Size() == S(0,0));
        CHECK_THAT(box.data, Eq({}));
        CHECK_THAT(box.to_string(AsciiStyle()), Eq({}));
    }

    SECTION("putchar")
    {
        SECTION("0")
        {
            box.putchar('x', 0, 0);

            INFO(box.data);
            CHECK(box.Size() == S(1,1));
            CHECK_THAT(box.data, Eq({"x"}));
            CHECK_THAT(box.to_string(AsciiStyle()), Eq({"x"}));
        }

        SECTION("offset")
        {
            box.putchar('x', 2, 1);

            INFO(box.data);
            CHECK(box.Size() == S(3,2));
            CHECK_THAT(box.data, Eq({"", "  x"}));
            CHECK_THAT(box.to_string(AsciiStyle()), Eq({"  x"}));
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
            CHECK_THAT(box.to_string(AsciiStyle()), Eq({"d"}));
        }

        SECTION("offset")
        {
            box.modchar(2, 1, [](char& c){c = 'd';});

            INFO(box.data);
            CHECK(box.Size() == S(3,2));
            CHECK_THAT(box.data, Eq({"", "  d"}));
            CHECK_THAT(box.to_string(AsciiStyle()), Eq({"  d"}));
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
            CHECK_THAT(box.to_string(AsciiStyle()), Eq({"dog"}));
        }

        SECTION("offset")
        {
            box.putline("dog", 1, 1);

            INFO(box.data);
            CHECK(box.Size() == S(4,2));
            CHECK_THAT(box.data, Eq({"", " dog"}));
            CHECK_THAT(box.to_string(AsciiStyle()), Eq({" dog"}));
        }

        SECTION("collision")
        {
            box.putline("doggo", 0, 0);
            box.putline( "dog", 1, 0);

            INFO(box.data);
            CHECK(box.Size() == S(5,1));
            CHECK_THAT(box.data, Eq({"ddogo"}));
            CHECK_THAT(box.to_string(AsciiStyle()), Eq({"ddogo"}));
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
            CHECK_THAT(box.to_string(AsciiStyle()), Eq({}));
        }

        SECTION("no change")
        {
            box.data = {"", "  a"};
            box.trim();

            INFO(box.data);
            CHECK(box.Size() == S(3,2));
            CHECK_THAT(box.data, Eq({"", "  a"}));
            CHECK_THAT(box.to_string(AsciiStyle()), Eq({"  a"}));
        }

        SECTION("change")
        {
            box.data = {"    ", "  a ", ""};
            box.trim();

            INFO(box.data);
            CHECK(box.Size() == S(3,2));
            CHECK_THAT(box.data, Eq({"", "  a"}));
            CHECK_THAT(box.to_string(AsciiStyle()), Eq({"  a"}));
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
            CHECK_THAT(box.to_string(AsciiStyle()), Eq({"---"}));
        }

        SECTION("h2")
        {
            box.hline(0, 0, 3, false, true);

            INFO(box.data);
            CHECK(box.Size() == S(3,1));
            CHECK_THAT(box.data, Eq({ascii({8, 12, 12})}));
            CHECK_THAT(box.to_string(AsciiStyle()), Eq({"---"}));
        }

        SECTION("h3")
        {
            box.hline(0, 0, 3, true, false);

            INFO(box.data);
            CHECK(box.Size() == S(3,1));
            CHECK_THAT(box.data, Eq({ascii({12, 12, 4})}));
            CHECK_THAT(box.to_string(AsciiStyle()), Eq({"---"}));
        }

        SECTION("h4")
        {
            box.hline(0, 0, 3, true, true);

            INFO(box.data);
            CHECK(box.Size() == S(3,1));
            CHECK_THAT(box.data, Eq({ascii({12, 12, 12})}));
            CHECK_THAT(box.to_string(AsciiStyle()), Eq({"---"}));
        }

        SECTION("v1")
        {
            box.vline(0, 0, 3, false, false);

            INFO(box.data);
            CHECK(box.Size() == S(1,3));
            CHECK_THAT(box.data, Eq({ascii({2}), ascii({3}), ascii({1})}));
            CHECK_THAT(box.to_string(AsciiStyle()), Eq({"|", "|", "|"}));
        }

        SECTION("v2")
        {
            box.vline(0, 0, 3, false, true);

            INFO(box.data);
            CHECK(box.Size() == S(1,3));
            CHECK_THAT(box.data, Eq({ascii({2}), ascii({3}), ascii({3})}));
            CHECK_THAT(box.to_string(AsciiStyle()), Eq({"|", "|", "|"}));
        }

        SECTION("v3")
        {
            box.vline(0, 0, 3, true, false);

            INFO(box.data);
            CHECK(box.Size() == S(1,3));
            CHECK_THAT(box.data, Eq({ascii({3}), ascii({3}), ascii({1})}));
            CHECK_THAT(box.to_string(AsciiStyle()), Eq({"|", "|", "|"}));
        }

        SECTION("v4")
        {
            box.vline(0, 0, 3, true, true);

            INFO(box.data);
            CHECK(box.Size() == S(1,3));
            CHECK_THAT(box.data, Eq({ascii({3}), ascii({3}), ascii({3})}));
            CHECK_THAT(box.to_string(AsciiStyle()), Eq({"|", "|", "|"}));
        }
    }
}

// todo(Gustav): How is this supposed to work?
TEST_CASE("tb_box")
{
    const std::vector<std::string> x_data = {"   ", " x ", "   "};
    const std::vector<std::string> abc_data = {"abc"};

    const TextBox empty;
    const auto x = TextBox{ x_data };
    const auto abc = TextBox{ abc_data };

    SECTION("putbox")
    {
        CHECK_THAT(empty.PutBoxCopy(0, 0, empty).to_string(AsciiStyle()), Eq({}));
        CHECK_THAT(empty.PutBoxCopy(0, 0, x).to_string(AsciiStyle()), Eq(x_data));
        CHECK_THAT(empty.PutBoxCopy(0, 0, abc).to_string(AsciiStyle()), Eq(abc_data));
        CHECK_THAT(empty.PutBoxCopy(1, 1, empty).to_string(AsciiStyle()), Eq({}));
        CHECK_THAT(empty.PutBoxCopy(1, 1, x).to_string(AsciiStyle()), Eq({"    ", "  x ", "    "}));
        CHECK_THAT(empty.PutBoxCopy(1, 1, empty).to_string(AsciiStyle()), Eq({}));
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

    CHECK_THAT(create_tree_graph(simple_tree, 130,
                [](const T& e) { return e.name; },
                [](const T& e) { return std::make_pair(e.children.cbegin(), e.children.cend()); },
                [](const T&  ) { return false; },
                [](const T&  ) { return false; },
                [](const T&  ) { return false; }).to_string(AsciiStyle()),
                Eq(simple_three_row));

    CHECK_THAT(create_tree_graph(simple_tree, 130,
                [](const T& e) { return e.name; },
                [](const T& e) { return std::make_pair(e.children.cbegin(), e.children.cend()); },
                [](const T&  ) { return false; },
                [](const T&  ) { return false; },
                [](const T&  ) { return true; }).to_string(AsciiStyle()),
                Eq(simple_four_row));
    
    CHECK_THAT(create_tree_graph(simple_tree, 130,
                [](const T& e) { return e.name; },
                [](const T& e) { return std::make_pair(e.children.cbegin(), e.children.cend()); },
                [](const T&  ) { return false; },
                [](const T&  ) { return true; },
                [](const T&  ) { return false; }).to_string(AsciiStyle()),
                Eq(simple_three_row));

    CHECK_THAT(create_tree_graph(simple_tree, 130,
                [](const T& e) { return e.name; },
                [](const T& e) { return std::make_pair(e.children.cbegin(), e.children.cend()); },
                [](const T&  ) { return false; },
                [](const T&  ) { return true; },
                [](const T&  ) { return true; }).to_string(AsciiStyle()),
                Eq(simple_four_row));


    CHECK_THAT(create_tree_graph(simple_tree, 130,
                [](const T& e) { return e.name; },
                [](const T& e) { return std::make_pair(e.children.cbegin(), e.children.cend()); },
                [](const T&  ) { return true; },
                [](const T&  ) { return false; },
                [](const T&  ) { return false; }).to_string(AsciiStyle()),
                Eq(simple_two_row));

    CHECK_THAT(create_tree_graph(simple_tree, 130,
                [](const T& e) { return e.name; },
                [](const T& e) { return std::make_pair(e.children.cbegin(), e.children.cend()); },
                [](const T&  ) { return true; },
                [](const T&  ) { return false; },
                [](const T&  ) { return true; }).to_string(AsciiStyle()),
                Eq(simple_four_row));
    
    CHECK_THAT(create_tree_graph(simple_tree, 130,
                [](const T& e) { return e.name; },
                [](const T& e) { return std::make_pair(e.children.cbegin(), e.children.cend()); },
                [](const T&  ) { return true; },
                [](const T&  ) { return true; },
                [](const T&  ) { return false; }).to_string(AsciiStyle()),
                Eq(simple_two_row));

    CHECK_THAT(create_tree_graph(simple_tree, 130,
                [](const T& e) { return e.name; },
                [](const T& e) { return std::make_pair(e.children.cbegin(), e.children.cend()); },
                [](const T&  ) { return true; },
                [](const T&  ) { return true; },
                [](const T&  ) { return true; }).to_string(AsciiStyle()),
                Eq(simple_four_row));
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
