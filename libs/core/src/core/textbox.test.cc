#include "core/textbox.h"

#include "tests/stringeq.h"

#include "catch.hpp"

#include <vector>
#include <string>

constexpr bool print_hex = false;

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
            if(first) { first = false; }
            else { s << ", "; }
            s << '"' << st << '"';

            if constexpr (print_hex)
            {
                s << "(";
                bool f = true;
                for(auto c: st)
                {
                    if(f) { f=false; }
                    else { s << " "; }
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
    std::pair<size_t, size_t> create_size(size_t a, size_t b)
    {
        return {a, b};
    }
}

TEST_CASE("tb_print")
{
    auto box = text_box::create_empty();

    SECTION("empty")
    {
        CHECK(box.get_size() == create_size(0, 0));
        CHECK(string_is_equal(box.to_string(ascii_style()), {}));
    }

    SECTION("putchar")
    {
        SECTION("0")
        {
            box.put_char(0, 0, 'x');

            CHECK(box.get_size() == create_size(1, 1));
            CHECK(string_is_equal(box.to_string(ascii_style()), {"x"}));
        }

        SECTION("offset")
        {
            box.put_char(2, 1, 'x');

            CHECK(box.get_size() == create_size(3, 2));
            CHECK(string_is_equal(box.to_string(ascii_style()), {"", "  x"}));
        }
    }

    SECTION("modchar")
    {
        SECTION("0")
        {
            box.mod_char(0, 0, [](char& c){c = 'd';});

            CHECK(box.get_size() == create_size(1, 1));
            CHECK(string_is_equal(box.to_string(ascii_style()), {"d"}));
        }

        SECTION("offset")
        {
            box.mod_char(2, 1, [](char& c){c = 'd';});

            CHECK(box.get_size() == create_size(3, 2));
            CHECK(string_is_equal(box.to_string(ascii_style()), {"", "  d"}));
        }
    }

    SECTION("putline")
    {
        SECTION("0")
        {
            box.put_string(0, 0, "dog");

            CHECK(box.get_size() == create_size(3, 1));
            CHECK(string_is_equal(box.to_string(ascii_style()), {"dog"}));
        }

        SECTION("offset")
        {
            box.put_string(1, 1, "dog");

            CHECK(box.get_size() == create_size(4, 2));
            CHECK(string_is_equal(box.to_string(ascii_style()), {"", " dog"}));
        }

        SECTION("collision")
        {
            box.put_string(0, 0, "doggo");
            box.put_string(1, 0, "dog");

            CHECK(box.get_size() == create_size(5, 1));
            CHECK(string_is_equal(box.to_string(ascii_style()), {"ddogo"}));
        }
    }

    // putbox

    SECTION("trim")
    {
        SECTION("empty")
        {
            box.trim();

            CHECK(box.get_size() == create_size(0, 0));
            CHECK(string_is_equal(box.to_string(ascii_style()), {}));
        }

        SECTION("no change")
        {
            box = text_box::create_from_strings({"", "  a"});
            box.trim();

            CHECK(box.get_size() == create_size(3, 2));
            CHECK(string_is_equal(box.to_string(ascii_style()), {"", "  a"}));
        }

        SECTION("change")
        {
            box = text_box::create_from_strings({"    ", "  a ", ""});
            box.trim();

            CHECK(box.get_size() == create_size(3, 2));
            CHECK(string_is_equal(box.to_string(ascii_style()), {"", "  a"}));
        }
    }
}

TEST_CASE("tb_arrows")
{
    const auto abc_style = text_box_style::create([](char c) {
        switch(c) {
        case bit_left:                                 return u8"a";
        case bit_right:                                return u8"b";
        case bit_left | bit_right:                     return u8"c";
        case bit_up:                                   return u8"d";
        case bit_down:                                 return u8"e";
        case bit_up | bit_down:                        return u8"f";
        case bit_left| bit_up:                         return u8"g";
        case bit_left | bit_down:                      return u8"h";
        case bit_right | bit_up:                       return u8"i";
        case bit_right | bit_down:                     return u8"j";
        case bit_left | bit_right | bit_up:            return u8"k";
        case bit_left | bit_right | bit_down:          return u8"l";
        case bit_left | bit_up | bit_down:             return u8"m";
        case bit_right | bit_up | bit_down:            return u8"n";
        case bit_left | bit_right | bit_up | bit_down: return u8"o";
        default:
            DIE("Invalid combination");
            return "?";
        }
    });

    auto box = text_box::create_empty();

    SECTION("all combos")
    {
        for(int i=1; i<16; i+=1)
        {
            box.put_char((i % 4), (i/4), static_cast<char>(i));
        }

        CHECK
        (
            string_is_equal
            (
                box.to_string(ascii_style()),
                {
                    " |||",
                    "-'.+",
                    "-`,+",
                    "-+++"
                }
            )
        );

        CHECK
        (
            string_is_equal
            (
                box.to_string(abc_style),
                {
                    " def",
                    "aghm",
                    "bijn",
                    "cklo"
                }
            )
        );
    }

    SECTION("lines")
    {
        SECTION("h1")
        {
            box.put_horizontal_line(0, 0, 3, false, false);

            CHECK(box.get_size() == create_size(3, 1));
            CHECK(string_is_equal(box.to_string(abc_style), {"bca"}));
        }

        SECTION("h2")
        {
            box.put_horizontal_line(0, 0, 3, false, true);

            CHECK(box.get_size() == create_size(3, 1));
            CHECK(string_is_equal(box.to_string(abc_style), {"bcc"}));
        }

        SECTION("h3")
        {
            box.put_horizontal_line(0, 0, 3, true, false);

            CHECK(box.get_size() == create_size(3, 1));
            CHECK(string_is_equal(box.to_string(abc_style), {"cca"}));
        }

        SECTION("h4")
        {
            box.put_horizontal_line(0, 0, 3, true, true);

            CHECK(box.get_size() == create_size(3, 1));
            CHECK(string_is_equal(box.to_string(abc_style), {"ccc"}));
        }

        SECTION("v1")
        {
            box.put_vertical_line(0, 0, 3, false, false);

            CHECK(box.get_size() == create_size(1, 3));
            CHECK(string_is_equal(box.to_string(abc_style), {"e", "f", "d"}));
        }

        SECTION("v2")
        {
            box.put_vertical_line(0, 0, 3, false, true);

            CHECK(box.get_size() == create_size(1, 3));
            CHECK(string_is_equal(box.to_string(abc_style), {"e", "f", "f"}));
        }

        SECTION("v3")
        {
            box.put_vertical_line(0, 0, 3, true, false);

            CHECK(box.get_size() == create_size(1, 3));
            CHECK(string_is_equal(box.to_string(abc_style), {"f", "f", "d"}));
        }

        SECTION("v4")
        {
            box.put_vertical_line(0, 0, 3, true, true);

            CHECK(box.get_size() == create_size(1, 3));
            CHECK(string_is_equal(box.to_string(abc_style), {"f", "f", "f"}));
        }
    }
}

TEST_CASE("tb_line on text")
{
    auto text = text_box::create_empty();
    text.put_string(1, 0, "d g");
    CHECK(string_is_equal(text.to_string(ascii_style()), {" d g"}));

    auto line = text_box::create_empty();
    line.put_horizontal_line(0, 0, 5, true, true);
    CHECK(string_is_equal(line.to_string(ascii_style()), {"-----"}));

    /// hrm... this is a weird behaviour...

    SECTION("text on line")
    {
        const auto r = text.put_box_copy(0, 0, line);
        CHECK(string_is_equal(r.to_string(ascii_style()), {"-----"}));
    }

    SECTION("line on text")
    {
        const auto r = line.put_box_copy(0, 0, text);
        CHECK(string_is_equal(r.to_string(ascii_style()), {"-d-g-"}));
    }
}

TEST_CASE("tb_line on line")
{
    auto hor = text_box::create_empty();
    hor.put_horizontal_line(0, 0, 1, true, true);
    CHECK(string_is_equal(hor.to_string(ascii_style()), {"-"}));

    auto vert = text_box::create_empty();
    vert.put_vertical_line(0, 0, 1, true, true);
    CHECK(string_is_equal(vert.to_string(ascii_style()), {"|"}));

    SECTION("hor on vert")
    {
        const auto r = vert.put_box_copy(0, 0, hor);
        CHECK(string_is_equal(r.to_string(ascii_style()), {"+"}));
    }

    SECTION("vert on hor")
    {
        const auto r = hor.put_box_copy(0, 0, vert);
        CHECK(string_is_equal(r.to_string(ascii_style()), {"+"}));
    }
}

// todo(Gustav): How is this supposed to work?
TEST_CASE("tb_box")
{
    const std::vector<std::string> x_data = {"   ", " x ", "   "};
    const std::vector<std::string> abc_data = {"abc"};

    const auto empty = text_box::create_empty();
    const auto x = text_box::create_from_strings(x_data);
    const auto abc = text_box::create_from_strings(abc_data);

    SECTION("putbox")
    {
        CHECK(string_is_equal(empty.put_box_copy(0, 0, empty).to_string(ascii_style()), {}));
        CHECK(string_is_equal(empty.put_box_copy(0, 0, x).to_string(ascii_style()), x_data));
        CHECK(string_is_equal(empty.put_box_copy(0, 0, abc).to_string(ascii_style()), abc_data));
        CHECK(string_is_equal(empty.put_box_copy(1, 1, empty).to_string(ascii_style()), {}));
        CHECK(string_is_equal(empty.put_box_copy(1, 1, x).to_string(ascii_style()), {"", "    ", "  x ", "    "}));
        CHECK(string_is_equal(empty.put_box_copy(1, 1, empty).to_string(ascii_style()), {}));
    }

    SECTION("empty")
    {
        CHECK(empty.get_horizontal_append_position(0, empty) == 0);
        CHECK(empty.get_vertical_append_position(0, empty) == 0);

        CHECK(empty.get_horizontal_append_position(0, x) == 0);
        CHECK(empty.get_vertical_append_position(0, x) == 0);

        CHECK(empty.get_horizontal_append_position(0, abc) == 0);
        CHECK(empty.get_vertical_append_position(0, abc) == 0);

        CHECK(empty.get_horizontal_append_position(1, empty) == 0);
        CHECK(empty.get_vertical_append_position(1, empty) == 0);

        CHECK(empty.get_horizontal_append_position(1, x) == 0);
        CHECK(empty.get_vertical_append_position(1, x) == 0);

        CHECK(empty.get_horizontal_append_position(1, abc) == 0);
        CHECK(empty.get_vertical_append_position(1, abc) == 0);
    }

    SECTION("x")
    {
        CHECK(x.get_horizontal_append_position(0, empty) == 0);
        CHECK(x.get_vertical_append_position(0, empty) == 0);

        CHECK(x.get_horizontal_append_position(0, x) == 1);
        CHECK(x.get_vertical_append_position(0, x) == 1);

        CHECK(x.get_horizontal_append_position(0, abc) == 0);
        CHECK(x.get_vertical_append_position(0, abc) == 2);

        CHECK(x.get_horizontal_append_position(1, empty) == 0);
        CHECK(x.get_vertical_append_position(1, empty) == 0);

        CHECK(x.get_horizontal_append_position(1, x) == 0);
        CHECK(x.get_vertical_append_position(1, x) == 0);

        CHECK(x.get_horizontal_append_position(1, abc) == 2);
        CHECK(x.get_vertical_append_position(1, abc) == 2);
    }

    SECTION("abc")
    {
        CHECK(abc.get_horizontal_append_position(0, empty) == 0);
        CHECK(abc.get_vertical_append_position(0, empty) == 0);

        CHECK(abc.get_horizontal_append_position(0, x) == 0);
        CHECK(abc.get_vertical_append_position(0, x) == 0);

        CHECK(abc.get_horizontal_append_position(0, abc) == 3);
        CHECK(abc.get_vertical_append_position(0, abc) == 1);

        CHECK(abc.get_horizontal_append_position(1, empty) == 0);
        CHECK(abc.get_vertical_append_position(1, empty) == 0);

        CHECK(abc.get_horizontal_append_position(1, x) == 0);
        CHECK(abc.get_vertical_append_position(1, x) == 0);

        CHECK(abc.get_horizontal_append_position(1, abc) == 0);
        CHECK(abc.get_vertical_append_position(1, abc) == 1);
    }
}

namespace
{
    struct node
    {
        std::string name;
        std::vector<node> children;

        node(const std::string& n) : name(n) {}
        node(const std::string& n, std::initializer_list<node> t) : name(n), children(t) {}
    };
}

TEST_CASE("tb_create_tree_graph")
{
    const auto simple_tree = node("a", {node{"1"}, node{"2"}, node{"3"} });

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

    CHECK
    (
        string_is_equal
        (
            text_box::create_tree_graph
            (
                simple_tree,
                130,
                [](const node &e) { return e.name; },
                [](const node &e)
                {
                    return std::make_pair(e.children.cbegin(), e.children.cend());
                },
                [](const node &) { return false; },
                [](const node &) { return false; }
            ).to_string(ascii_style()),
            simple_three_row
        )
    );

    CHECK
    (
        string_is_equal
        (
            text_box::create_tree_graph
            (
                simple_tree,
                130,
                [](const node &e) { return e.name; },
                [](const node &e)
                {
                    return std::make_pair(e.children.cbegin(), e.children.cend());
                },
                [](const node &) { return false; },
                [](const node &) { return true; }
            ).to_string(ascii_style()),
            simple_three_row
        )
    );


    CHECK
    (
        string_is_equal
        (
            text_box::create_tree_graph
            (
                simple_tree,
                130,
                [](const node &e) { return e.name; },
                [](const node &e)
                {
                    return std::make_pair(e.children.cbegin(), e.children.cend());
                },
                [](const node &) { return true; },
                [](const node &) { return false; }
            ).to_string(ascii_style()),
            simple_two_row
        )
    );

    CHECK
    (
        string_is_equal
        (
            text_box::create_tree_graph
            (
                simple_tree,
                130,
                [](const node &e) { return e.name; },
                [](const node &e)
                {
                    return std::make_pair(e.children.cbegin(), e.children.cend());
                },
                [](const node &) { return true; },
                [](const node &) { return true; }
            ).to_string(ascii_style()),
simple_two_row
)
);
}


TEST_CASE("tb_tolkien")
{
    const auto tolkien_tree = node
    {
        "Tolkien characters",
        {
                node
            {
                "Heroes",
                {
                        node
                    {
                        "Humans",
                        {
                                node{"Aragon"},
                                node{"Boromir"}
                        }
                    },
                        node
                    {
                        "Hobbits",
                        {
                                node{"Frodo"},
                                node{"Samwise"},
                                node{"Merry"},
                                node{"Pippin"},
                        }
                    },
                        node
                    {
                        "Other",
                        {
                                node{"Legolas"},
                                node{"Gandalf"},
                                node{"Gimli"}
                        }
                    }
                }
            },
                node
            {
                "Enemies",
                {
                        node{"Sauron"},
                        node{"orcs"},
                        node{"Sauruman"}
                }
            }
        }
    };
    CHECK
    (
        string_is_equal
        (
            text_box::create_tree_graph
            (
                tolkien_tree,
                130,
                [](const node &e) { return e.name; },
                [](const node &e)
                {
                    return std::make_pair(e.children.cbegin(), e.children.cend());
                },
                [](const node &e) { return !e.children.empty(); },
                [](const node &) { return true; }
            ).to_string(ascii_style()),
            {
                "Tolkien characters",
                "`-+----------------------------------------------------------------------------------.",
                "  Heroes--+------------------+----------------------------------.                    Enemies--+-------+-----.",
                "          Humans--+-------.  Hobbits--+------+--------+------.  Other--+--------+--------.    Sauron  orcs  Sauruman",
                "                  Aragon  Boromir     Frodo  Samwise  Merry  Pippin    Legolas  Gandalf  Gimli"
            }
        )
    );
}
