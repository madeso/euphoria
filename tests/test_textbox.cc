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
}

TEST_CASE("tb_print")
{
    auto box = TextBox::Empty();

    SECTION("empty")
    {
        CHECK(box.Size() == S(0,0));
        CHECK(StringEq(box.ToString(AsciiStyle()), {}));
    }

    SECTION("putchar")
    {
        SECTION("0")
        {
            box.PutChar(0, 0, 'x');

            CHECK(box.Size() == S(1,1));
            CHECK(StringEq(box.ToString(AsciiStyle()), {"x"}));
        }

        SECTION("offset")
        {
            box.PutChar(2, 1, 'x');

            CHECK(box.Size() == S(3,2));
            CHECK(StringEq(box.ToString(AsciiStyle()), {"", "  x"}));
        }
    }

    SECTION("modchar")
    {
        SECTION("0")
        {
            box.ModChar(0, 0, [](char& c){c = 'd';});

            CHECK(box.Size() == S(1,1));
            CHECK(StringEq(box.ToString(AsciiStyle()), {"d"}));
        }

        SECTION("offset")
        {
            box.ModChar(2, 1, [](char& c){c = 'd';});

            CHECK(box.Size() == S(3,2));
            CHECK(StringEq(box.ToString(AsciiStyle()), {"", "  d"}));
        }
    }

    SECTION("putline")
    {
        SECTION("0")
        {
            box.PutString(0, 0, "dog");

            CHECK(box.Size() == S(3,1));
            CHECK(StringEq(box.ToString(AsciiStyle()), {"dog"}));
        }

        SECTION("offset")
        {
            box.PutString(1, 1, "dog");

            CHECK(box.Size() == S(4,2));
            CHECK(StringEq(box.ToString(AsciiStyle()), {"", " dog"}));
        }

        SECTION("collision")
        {
            box.PutString(0, 0, "doggo");
            box.PutString(1, 0, "dog");

            CHECK(box.Size() == S(5,1));
            CHECK(StringEq(box.ToString(AsciiStyle()), {"ddogo"}));
        }
    }

    // putbox

    SECTION("trim")
    {
        SECTION("empty")
        {
            box.Trim();

            CHECK(box.Size() == S(0,0));
            CHECK(StringEq(box.ToString(AsciiStyle()), {}));
        }

        SECTION("no change")
        {
            box = TextBox::FromString({"", "  a"});
            box.Trim();

            CHECK(box.Size() == S(3,2));
            CHECK(StringEq(box.ToString(AsciiStyle()), {"", "  a"}));
        }

        SECTION("change")
        {
            box = TextBox::FromString({"    ", "  a ", ""});
            box.Trim();

            CHECK(box.Size() == S(3,2));
            CHECK(StringEq(box.ToString(AsciiStyle()), {"", "  a"}));
        }
    }
}

TEST_CASE("tb_arrows")
{
    const auto abc_style = TextBoxStyle::Create([](char c) {
        switch(c) {
        case BIT_LEFT:                                 return u8"a";
        case BIT_RIGHT:                                return u8"b";
        case BIT_LEFT | BIT_RIGHT:                     return u8"c";
        case BIT_UP:                                   return u8"d";
        case BIT_DOWN:                                 return u8"e";
        case BIT_UP | BIT_DOWN:                        return u8"f";
        case BIT_LEFT| BIT_UP:                         return u8"g";
        case BIT_LEFT | BIT_DOWN:                      return u8"h";
        case BIT_RIGHT | BIT_UP:                       return u8"i";
        case BIT_RIGHT | BIT_DOWN:                     return u8"j";
        case BIT_LEFT | BIT_RIGHT | BIT_UP:            return u8"k";
        case BIT_LEFT | BIT_RIGHT | BIT_DOWN:          return u8"l";
        case BIT_LEFT | BIT_UP | BIT_DOWN:             return u8"m";
        case BIT_RIGHT | BIT_UP | BIT_DOWN:            return u8"n";
        case BIT_LEFT | BIT_RIGHT | BIT_UP | BIT_DOWN: return u8"o";
        default:
            DIE("Invalid combination");
            return "?";
        }
    });

    auto box = TextBox::Empty();

    SECTION("all combos")
    {
        for(int i=1; i<16; i+=1)
        {
            box.PutChar((i % 4), (i/4), static_cast<char>(i));
        }

        CHECK(StringEq(box.ToString(AsciiStyle()),
        {
            " |||",
            "-'.+",
            "-`,+",
            "-+++"
        }));

        CHECK(StringEq(box.ToString(abc_style),
        {
            " def",
            "aghm",
            "bijn",
            "cklo"
        }));
    }

    SECTION("lines")
    {
        SECTION("h1")
        {
            box.PutHorizontalLine(0, 0, 3, false, false);

            CHECK(box.Size() == S(3,1));
            CHECK(StringEq(box.ToString(abc_style), {"bca"}));
        }

        SECTION("h2")
        {
            box.PutHorizontalLine(0, 0, 3, false, true);

            CHECK(box.Size() == S(3,1));
            CHECK(StringEq(box.ToString(abc_style), {"bcc"}));
        }

        SECTION("h3")
        {
            box.PutHorizontalLine(0, 0, 3, true, false);

            CHECK(box.Size() == S(3,1));
            CHECK(StringEq(box.ToString(abc_style), {"cca"}));
        }

        SECTION("h4")
        {
            box.PutHorizontalLine(0, 0, 3, true, true);

            CHECK(box.Size() == S(3,1));
            CHECK(StringEq(box.ToString(abc_style), {"ccc"}));
        }

        SECTION("v1")
        {
            box.PutVerticalLine(0, 0, 3, false, false);

            CHECK(box.Size() == S(1,3));
            CHECK(StringEq(box.ToString(abc_style), {"e", "f", "d"}));
        }

        SECTION("v2")
        {
            box.PutVerticalLine(0, 0, 3, false, true);

            CHECK(box.Size() == S(1,3));
            CHECK(StringEq(box.ToString(abc_style), {"e", "f", "f"}));
        }

        SECTION("v3")
        {
            box.PutVerticalLine(0, 0, 3, true, false);

            CHECK(box.Size() == S(1,3));
            CHECK(StringEq(box.ToString(abc_style), {"f", "f", "d"}));
        }

        SECTION("v4")
        {
            box.PutVerticalLine(0, 0, 3, true, true);

            CHECK(box.Size() == S(1,3));
            CHECK(StringEq(box.ToString(abc_style), {"f", "f", "f"}));
        }
    }
}

TEST_CASE("tb_line on text")
{
    auto text = TextBox::Empty();
    text.PutString(1, 0, "d g");
    CHECK(StringEq(text.ToString(AsciiStyle()), {" d g"}));

    auto line = TextBox::Empty();
    line.PutHorizontalLine(0, 0, 5, true, true);
    CHECK(StringEq(line.ToString(AsciiStyle()), {"-----"}));

    /// hrm... this is a weird behaviour...

    SECTION("text on line")
    {
        const auto r = text.PutBoxCopy(0, 0, line);
        CHECK(StringEq(r.ToString(AsciiStyle()), {"-----"}));
    }

    SECTION("line on text")
    {
        const auto r = line.PutBoxCopy(0, 0, text);
        CHECK(StringEq(r.ToString(AsciiStyle()), {"-d-g-"}));
    }
}

TEST_CASE("tb_line on line")
{
    auto hor = TextBox::Empty();
    hor.PutHorizontalLine(0, 0, 1, true, true);
    CHECK(StringEq(hor.ToString(AsciiStyle()), {"-"}));

    auto vert = TextBox::Empty();
    vert.PutVerticalLine(0, 0, 1, true, true);
    CHECK(StringEq(vert.ToString(AsciiStyle()), {"|"}));

    SECTION("hor on vert")
    {
        const auto r = vert.PutBoxCopy(0, 0, hor);
        CHECK(StringEq(r.ToString(AsciiStyle()), {"|"}));
    }

    SECTION("vert on hor")
    {
        const auto r = hor.PutBoxCopy(0, 0, vert);
        CHECK(StringEq(r.ToString(AsciiStyle()), {"-"}));
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
        CHECK(StringEq(empty.PutBoxCopy(0, 0, empty).ToString(AsciiStyle()), {}));
        CHECK(StringEq(empty.PutBoxCopy(0, 0, x).ToString(AsciiStyle()), x_data));
        CHECK(StringEq(empty.PutBoxCopy(0, 0, abc).ToString(AsciiStyle()), abc_data));
        CHECK(StringEq(empty.PutBoxCopy(1, 1, empty).ToString(AsciiStyle()), {}));
        CHECK(StringEq(empty.PutBoxCopy(1, 1, x).ToString(AsciiStyle()), {"", "    ", "  x ", "    "}));
        CHECK(StringEq(empty.PutBoxCopy(1, 1, empty).ToString(AsciiStyle()), {}));
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

    CHECK(StringEq(TextBox::CreateTreeGraph(simple_tree, 130,
                [](const T& e) { return e.name; },
                [](const T& e) { return std::make_pair(e.children.cbegin(), e.children.cend()); },
                [](const T&  ) { return false; },
                [](const T&  ) { return false; },
                [](const T&  ) { return false; }).ToString(AsciiStyle()),
                simple_three_row));

    CHECK(StringEq(TextBox::CreateTreeGraph(simple_tree, 130,
                [](const T& e) { return e.name; },
                [](const T& e) { return std::make_pair(e.children.cbegin(), e.children.cend()); },
                [](const T&  ) { return false; },
                [](const T&  ) { return false; },
                [](const T&  ) { return true; }).ToString(AsciiStyle()),
                simple_four_row));
    
    CHECK(StringEq(TextBox::CreateTreeGraph(simple_tree, 130,
                [](const T& e) { return e.name; },
                [](const T& e) { return std::make_pair(e.children.cbegin(), e.children.cend()); },
                [](const T&  ) { return false; },
                [](const T&  ) { return true; },
                [](const T&  ) { return false; }).ToString(AsciiStyle()),
                simple_three_row));

    CHECK(StringEq(TextBox::CreateTreeGraph(simple_tree, 130,
                [](const T& e) { return e.name; },
                [](const T& e) { return std::make_pair(e.children.cbegin(), e.children.cend()); },
                [](const T&  ) { return false; },
                [](const T&  ) { return true; },
                [](const T&  ) { return true; }).ToString(AsciiStyle()),
                simple_four_row));


    CHECK(StringEq(TextBox::CreateTreeGraph(simple_tree, 130,
                [](const T& e) { return e.name; },
                [](const T& e) { return std::make_pair(e.children.cbegin(), e.children.cend()); },
                [](const T&  ) { return true; },
                [](const T&  ) { return false; },
                [](const T&  ) { return false; }).ToString(AsciiStyle()),
                simple_two_row));

    CHECK(StringEq(TextBox::CreateTreeGraph(simple_tree, 130,
                [](const T& e) { return e.name; },
                [](const T& e) { return std::make_pair(e.children.cbegin(), e.children.cend()); },
                [](const T&  ) { return true; },
                [](const T&  ) { return false; },
                [](const T&  ) { return true; }).ToString(AsciiStyle()),
                simple_four_row));
    
    CHECK(StringEq(TextBox::CreateTreeGraph(simple_tree, 130,
                [](const T& e) { return e.name; },
                [](const T& e) { return std::make_pair(e.children.cbegin(), e.children.cend()); },
                [](const T&  ) { return true; },
                [](const T&  ) { return true; },
                [](const T&  ) { return false; }).ToString(AsciiStyle()),
                simple_two_row));

    CHECK(StringEq(TextBox::CreateTreeGraph(simple_tree, 130,
                [](const T& e) { return e.name; },
                [](const T& e) { return std::make_pair(e.children.cbegin(), e.children.cend()); },
                [](const T&  ) { return true; },
                [](const T&  ) { return true; },
                [](const T&  ) { return true; }).ToString(AsciiStyle()),
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
    CHECK(StringEq(TextBox::CreateTreeGraph(tolkien_tree, 130,
                [](const T& e) { return e.name; },
                [](const T& e) { return std::make_pair(e.children.cbegin(), e.children.cend()); },
                [](const T& e) { return e.children.size() >= 1; },
                [](const T&  ) { return true; },
                [](const T&  ) { return false; }).ToString(AsciiStyle()),
                {
  "Tolkien characters",
  "`-+----------------------------------------------------------------------------------.",
  "  Heroes--+------------------+----------------------------------.                    Enemies--+-------+-----.",
  "          Humans--+-------.  Hobbits--+------+--------+------.  Other--+--------+--------.    Sauron  orcs  Sauruman",
  "                  Aragon  Boromir     Frodo  Samwise  Merry  Pippin    Legolas  Gandalf  Gimli"
                }));
}
