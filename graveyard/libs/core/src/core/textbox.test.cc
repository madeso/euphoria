#include "core/textbox.h"

#include "tests/stringeq.h"

#include "catch2/catch_all.hpp"


using namespace eu;
using namespace eu::core;
using namespace eu::tests;


namespace
{
    std::pair<int, int> create_size(int a, int b)
    {
        return {a, b};
    }
}

TEST_CASE("tb_print")
{
    auto box = TextBox::create_empty();

    SECTION("empty")
    {
        CHECK(box.get_size() == create_size(0, 0));
        CHECK(is_string_equal(box.to_string(ascii_style), {}));
    }

    SECTION("putchar")
    {
        SECTION("0")
        {
            box.put_char(0, 0, 'x');

            CHECK(box.get_size() == create_size(1, 1));
            CHECK(is_string_equal(box.to_string(ascii_style), {"x"}));
        }

        SECTION("offset")
        {
            box.put_char(2, 1, 'x');

            CHECK(box.get_size() == create_size(3, 2));
            CHECK(is_string_equal(box.to_string(ascii_style), {"", "  x"}));
        }
    }

    SECTION("modchar")
    {
        SECTION("0")
        {
            box.mod_char(0, 0, [](char& c){c = 'd';});

            CHECK(box.get_size() == create_size(1, 1));
            CHECK(is_string_equal(box.to_string(ascii_style), {"d"}));
        }

        SECTION("offset")
        {
            box.mod_char(2, 1, [](char& c){c = 'd';});

            CHECK(box.get_size() == create_size(3, 2));
            CHECK(is_string_equal(box.to_string(ascii_style), {"", "  d"}));
        }
    }

    SECTION("putline")
    {
        SECTION("0")
        {
            box.put_string(0, 0, "dog");

            CHECK(box.get_size() == create_size(3, 1));
            CHECK(is_string_equal(box.to_string(ascii_style), {"dog"}));
        }

        SECTION("offset")
        {
            box.put_string(1, 1, "dog");

            CHECK(box.get_size() == create_size(4, 2));
            CHECK(is_string_equal(box.to_string(ascii_style), {"", " dog"}));
        }

        SECTION("collision")
        {
            box.put_string(0, 0, "doggo");
            box.put_string(1, 0, "dog");

            CHECK(box.get_size() == create_size(5, 1));
            CHECK(is_string_equal(box.to_string(ascii_style), {"ddogo"}));
        }
    }

    // putbox

    SECTION("trim")
    {
        SECTION("empty")
        {
            box.trim();

            CHECK(box.get_size() == create_size(0, 0));
            CHECK(is_string_equal(box.to_string(ascii_style), {}));
        }

        SECTION("no change")
        {
            box = TextBox::create_from_strings({"", "  a"});
            box.trim();

            CHECK(box.get_size() == create_size(3, 2));
            CHECK(is_string_equal(box.to_string(ascii_style), {"", "  a"}));
        }

        SECTION("change")
        {
            box = TextBox::create_from_strings({"    ", "  a ", ""});
            box.trim();

            CHECK(box.get_size() == create_size(3, 2));
            CHECK(is_string_equal(box.to_string(ascii_style), {"", "  a"}));
        }
    }
}

TEST_CASE("tb_arrows")
{
    const auto abc_style = TextBoxStyle{ [](char c) {
        switch (c) {
        case bit_left:                                 return "a";
        case bit_right:                                return "b";
        case bit_left | bit_right:                     return "c";
        case bit_up:                                   return "d";
        case bit_down:                                 return "e";
        case bit_up | bit_down:                        return "f";
        case bit_left | bit_up:                         return "g";
        case bit_left | bit_down:                      return "h";
        case bit_right | bit_up:                       return "i";
        case bit_right | bit_down:                     return "j";
        case bit_left | bit_right | bit_up:            return "k";
        case bit_left | bit_right | bit_down:          return "l";
        case bit_left | bit_up | bit_down:             return "m";
        case bit_right | bit_up | bit_down:            return "n";
        case bit_left | bit_right | bit_up | bit_down: return "o";
        default:
            DIE("Invalid combination");
            return "?";
        }
    }};

    auto box = TextBox::create_empty();

    SECTION("all combos")
    {
        for(int combo_index=1; combo_index<16; combo_index+=1)
        {
            box.put_char((combo_index % 4), (combo_index/4), static_cast<char>(combo_index));
        }

        CHECK
        (
            is_string_equal
            (
                box.to_string(ascii_style),
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
            is_string_equal
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
            CHECK(is_string_equal(box.to_string(abc_style), {"bca"}));
        }

        SECTION("h2")
        {
            box.put_horizontal_line(0, 0, 3, false, true);

            CHECK(box.get_size() == create_size(3, 1));
            CHECK(is_string_equal(box.to_string(abc_style), {"bcc"}));
        }

        SECTION("h3")
        {
            box.put_horizontal_line(0, 0, 3, true, false);

            CHECK(box.get_size() == create_size(3, 1));
            CHECK(is_string_equal(box.to_string(abc_style), {"cca"}));
        }

        SECTION("h4")
        {
            box.put_horizontal_line(0, 0, 3, true, true);

            CHECK(box.get_size() == create_size(3, 1));
            CHECK(is_string_equal(box.to_string(abc_style), {"ccc"}));
        }

        SECTION("v1")
        {
            box.put_vertical_line(0, 0, 3, false, false);

            CHECK(box.get_size() == create_size(1, 3));
            CHECK(is_string_equal(box.to_string(abc_style), {"e", "f", "d"}));
        }

        SECTION("v2")
        {
            box.put_vertical_line(0, 0, 3, false, true);

            CHECK(box.get_size() == create_size(1, 3));
            CHECK(is_string_equal(box.to_string(abc_style), {"e", "f", "f"}));
        }

        SECTION("v3")
        {
            box.put_vertical_line(0, 0, 3, true, false);

            CHECK(box.get_size() == create_size(1, 3));
            CHECK(is_string_equal(box.to_string(abc_style), {"f", "f", "d"}));
        }

        SECTION("v4")
        {
            box.put_vertical_line(0, 0, 3, true, true);

            CHECK(box.get_size() == create_size(1, 3));
            CHECK(is_string_equal(box.to_string(abc_style), {"f", "f", "f"}));
        }
    }
}

TEST_CASE("tb_line on text")
{
    auto text = TextBox::create_empty();
    text.put_string(1, 0, "d g");
    CHECK(is_string_equal(text.to_string(ascii_style), {" d g"}));

    auto line = TextBox::create_empty();
    line.put_horizontal_line(0, 0, 5, true, true);
    CHECK(is_string_equal(line.to_string(ascii_style), {"-----"}));

    /// hrm... this is a weird behaviour...

    SECTION("text on line")
    {
        const auto r = text.put_box_copy(0, 0, line);
        CHECK(is_string_equal(r.to_string(ascii_style), {"-----"}));
    }

    SECTION("line on text")
    {
        const auto r = line.put_box_copy(0, 0, text);
        CHECK(is_string_equal(r.to_string(ascii_style), {"-d-g-"}));
    }
}

TEST_CASE("tb_line on line")
{
    auto hor = TextBox::create_empty();
    hor.put_horizontal_line(0, 0, 1, true, true);
    CHECK(is_string_equal(hor.to_string(ascii_style), {"-"}));

    auto vert = TextBox::create_empty();
    vert.put_vertical_line(0, 0, 1, true, true);
    CHECK(is_string_equal(vert.to_string(ascii_style), {"|"}));

    SECTION("hor on vert")
    {
        const auto r = vert.put_box_copy(0, 0, hor);
        CHECK(is_string_equal(r.to_string(ascii_style), {"+"}));
    }

    SECTION("vert on hor")
    {
        const auto r = hor.put_box_copy(0, 0, vert);
        CHECK(is_string_equal(r.to_string(ascii_style), {"+"}));
    }
}

// todo(Gustav): How is this supposed to work?
TEST_CASE("tb_box")
{
    const std::vector<std::string> x_data = {"   ", " x ", "   "};
    const std::vector<std::string> abc_data = {"abc"};

    const auto empty = TextBox::create_empty();
    const auto x = TextBox::create_from_strings(x_data);
    const auto abc = TextBox::create_from_strings(abc_data);

    SECTION("putbox")
    {
        CHECK(is_string_equal(empty.put_box_copy(0, 0, empty).to_string(ascii_style), {}));
        CHECK(is_string_equal(empty.put_box_copy(0, 0, x).to_string(ascii_style), x_data));
        CHECK(is_string_equal(empty.put_box_copy(0, 0, abc).to_string(ascii_style), abc_data));
        CHECK(is_string_equal(empty.put_box_copy(1, 1, empty).to_string(ascii_style), {}));
        CHECK(is_string_equal(empty.put_box_copy(1, 1, x).to_string(ascii_style), {"", "    ", "  x ", "    "}));
        CHECK(is_string_equal(empty.put_box_copy(1, 1, empty).to_string(ascii_style), {}));
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
    struct Node
    {
        std::string name;
        std::vector<Node> children;

        Node(const std::string& n) : name(n) {}
        Node(const std::string& n, std::initializer_list<Node> t) : name(n), children(t) {}
    };
}

TEST_CASE("tb_create_tree_graph")
{
    const auto simple_tree = Node("a", {Node{"1"}, Node{"2"}, Node{"3"} });

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
        is_string_equal
        (
            TextBox::create_tree_graph
            (
                simple_tree,
                130,
                [](const Node &e) { return e.name; },
                [](const Node &e)
                {
                    return std::make_pair(e.children.cbegin(), e.children.cend());
                },
                [](const Node &) { return false; },
                [](const Node &) { return false; }
            ).to_string(ascii_style),
            simple_three_row
        )
    );

    CHECK
    (
        is_string_equal
        (
            TextBox::create_tree_graph
            (
                simple_tree,
                130,
                [](const Node &e) { return e.name; },
                [](const Node &e)
                {
                    return std::make_pair(e.children.cbegin(), e.children.cend());
                },
                [](const Node &) { return false; },
                [](const Node &) { return true; }
            ).to_string(ascii_style),
            simple_three_row
        )
    );


    CHECK
    (
        is_string_equal
        (
            TextBox::create_tree_graph
            (
                simple_tree,
                130,
                [](const Node &e) { return e.name; },
                [](const Node &e)
                {
                    return std::make_pair(e.children.cbegin(), e.children.cend());
                },
                [](const Node &) { return true; },
                [](const Node &) { return false; }
            ).to_string(ascii_style),
            simple_two_row
        )
    );

    CHECK
    (
        is_string_equal
        (
            TextBox::create_tree_graph
            (
                simple_tree,
                130,
                [](const Node &e) { return e.name; },
                [](const Node &e)
                {
                    return std::make_pair(e.children.cbegin(), e.children.cend());
                },
                [](const Node &) { return true; },
                [](const Node &) { return true; }
            ).to_string(ascii_style),
simple_two_row
)
);
}


TEST_CASE("tb_tolkien")
{
    const auto tolkien_tree = Node
    {
        "Tolkien characters",
        {
                Node
            {
                "Heroes",
                {
                        Node
                    {
                        "Humans",
                        {
                                Node{"Aragon"},
                                Node{"Boromir"}
                        }
                    },
                        Node
                    {
                        "Hobbits",
                        {
                                Node{"Frodo"},
                                Node{"Samwise"},
                                Node{"Merry"},
                                Node{"Pippin"},
                        }
                    },
                        Node
                    {
                        "Other",
                        {
                                Node{"Legolas"},
                                Node{"Gandalf"},
                                Node{"Gimli"}
                        }
                    }
                }
            },
                Node
            {
                "Enemies",
                {
                        Node{"Sauron"},
                        Node{"orcs"},
                        Node{"Sauruman"}
                }
            }
        }
    };
    CHECK
    (
        is_string_equal
        (
            TextBox::create_tree_graph
            (
                tolkien_tree,
                130,
                [](const Node &e) { return e.name; },
                [](const Node &e)
                {
                    return std::make_pair(e.children.cbegin(), e.children.cend());
                },
                [](const Node &e) { return !e.children.empty(); },
                [](const Node &) { return true; }
            ).to_string(ascii_style),
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
