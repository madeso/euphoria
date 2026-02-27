#include "base/rect.h"

#include "catch2/catch_all.hpp"

#include "tests/equaler.h"

using namespace eu;
using namespace eu::tests;

FalseString is_equal(const Rect& lhs, const Rect& rhs)
{
    return Equaler{lhs, rhs}
        .add("left", [](const auto& r) { return r.left; }, machine_epsilon)
        .add("right", [](const auto& r) { return r.right; }, machine_epsilon)
        .add("bottom", [](const auto& r) { return r.bottom; }, machine_epsilon)
        .add("top", [](const auto& r) { return r.top; }, machine_epsilon)
        .resolve();
}

TEST_CASE("rect-cut", "[rect]")
{
    auto rect = Rect::from_bottom_left_size({ 0.0, 0.0 }, { 10.0, 10.0 });

    SECTION("cut-left")
    {
        /*
            +---+-------------------+
            | L |   Remaining       |
            | E |                   |
            | F |                   |
            | T |                   |
            +---+-------------------+
            0   1                   10
        */
        const auto left = rect.cut_left(1.0);

        CHECK(is_equal(left, Rect::from_left_right_bottom_top(0.0, 1.0, 0.0, 10.0)));
        CHECK(is_equal(rect, Rect::from_left_right_bottom_top(1.0, 10.0, 0.0, 10.0)));
    }

    SECTION("cut-right")
    {
        /*
            +--------------------+--+
            |   Remaining       | R |
            |                   | I |
            |                   | G |
            |                   | H |
            |                   | T |
            +-------------------+---+
            0                   9   10
        */
        const auto right = rect.cut_right(1.0);
        CHECK(is_equal(right, Rect::from_left_right_bottom_top(9.0, 10.0, rect.bottom, rect.top)));
        CHECK(is_equal(rect, Rect::from_left_right_bottom_top(0.0, 9.0, 0.0, 10.0)));
    }

    SECTION("cut-top")
    {
        /*
            +----------------------+ 10
            |         TOP          |
            +----------------------+ 9
            |                      |
            |       Remaining      |
            |                      |
            +----------------------+ 0
        */
        const auto top = rect.cut_top(1.0);
        CHECK(is_equal(top, Rect::from_left_right_bottom_top(0.0, 10.0, 9.0, 10.0)));
        CHECK(is_equal(rect, Rect::from_left_right_bottom_top(0.0, 10.0, 0.0, 9.0)));
    }

    SECTION("cut-bottom")
    {
        /*
            +----------------------+ 10
            |                      |
            |      Remaining       |
            |                      |
            +----------------------+ 1
            |       BOTTOM         |
            +----------------------+ 0
        */
        const auto bottom = rect.cut_bottom(1.0);
        CHECK(is_equal(bottom, Rect::from_left_right_bottom_top(0.0, 10.0, 0.0, 1.0)));
        CHECK(is_equal(rect, Rect::from_left_right_bottom_top(0.0, 10.0, 1.0, 10.0)));
    }
}


TEST_CASE("rect-cut-spacing", "[rect]")
{
    auto rect = Rect::from_bottom_left_size({ 0.0, 0.0 }, { 10.0, 10.0 });

    SECTION("cut-left")
    {
        /*
            +---+   +------------------+
            | L |   |  Remaining       |
            | E |   |                  |
            | F |   |                  |
            | T |   |                  |
            +---+   +------------------+
            0   1   2                  10
        */
        const auto left = rect.cut_left(1.0, 1.0);

        CHECK(is_equal(left, Rect::from_left_right_bottom_top(0.0, 1.0, 0.0, 10.0)));
        CHECK(is_equal(rect, Rect::from_left_right_bottom_top(2.0, 10.0, 0.0, 10.0)));
    }

    SECTION("cut-right")
    {
        /*
            +----------------+   +---+
            |   Remaining    |   | R |
            |                |   | I |
            |                |   | G |
            |                |   | H |
            |                |   | T |
            +----------------+   +---+
            0                8   9   10
        */
        const auto right = rect.cut_right(1.0, 1.0);
        CHECK(is_equal(right, Rect::from_left_right_bottom_top(9.0, 10.0, rect.bottom, rect.top)));
        CHECK(is_equal(rect, Rect::from_left_right_bottom_top(0.0, 8.0, 0.0, 10.0)));
    }

    SECTION("cut-top")
    {
        /*
            +----------------------+ 10
            |         TOP          |
            +----------------------+ 9
                                    
            +----------------------+ 8
            |                      |
            |       Remaining      |
            |                      |
            +----------------------+ 0
        */
        const auto top = rect.cut_top(1.0, 1.0);
        CHECK(is_equal(top, Rect::from_left_right_bottom_top(0.0, 10.0, 9.0, 10.0)));
        CHECK(is_equal(rect, Rect::from_left_right_bottom_top(0.0, 10.0, 0.0, 8.0)));
    }

    SECTION("cut-bottom")
    {
        /*
            +----------------------+ 10
            |                      |
            |      Remaining       |
            |                      |
            +----------------------+ 2
                                    
            +----------------------+ 1
            |       BOTTOM         |
            +----------------------+ 0
        */
        const auto bottom = rect.cut_bottom(1.0, 1.0);
        CHECK(is_equal(bottom, Rect::from_left_right_bottom_top(0.0, 10.0, 0.0, 1.0)));
        CHECK(is_equal(rect, Rect::from_left_right_bottom_top(0.0, 10.0, 2.0, 10.0)));
    }
}


TEST_CASE("rect-get", "[rect]")
{
    const auto rect = Rect::from_bottom_left_size({ 0.0, 0.0 }, { 10.0, 10.0 });

    SECTION("get-left")
    {
        /*
            +---+-------------------+
            | L |   Original        |
            | E |                   |
            | F |                   |
            | T |                   |
            +---+-------------------+
            0   1                   10
        */
        const auto left = rect.get_left(1.0);
        CHECK(is_equal(left, Rect::from_left_right_bottom_top(0.0, 1.0, 0.0, 10.0)));
    }

    SECTION("get-right")
    {
        /*
            +--------------------+--+
            |   Original        | R |
            |                   | I |
            |                   | G |
            |                   | H |
            |                   | T |
            +-------------------+---+
            0                   9   10
        */
        const auto right = rect.get_right(1.0);
        CHECK(is_equal(right, Rect::from_left_right_bottom_top(9.0, 10.0, 0.0, 10.0)));
    }

    SECTION("get-top")
    {
        /*
            +----------------------+ 10
            |         TOP          |
            +----------------------+ 9
            |                      |
            |       Original       |
            |                      |
            +----------------------+ 0
        */
        const auto top = rect.get_top(1.0);
        CHECK(is_equal(top, Rect::from_left_right_bottom_top(0.0, 10.0, 9.0, 10.0)));
    }

    SECTION("get-bottom")
    {
        /*
            +----------------------+ 10
            |                      |
            |      Original        |
            |                      |
            +----------------------+ 1
            |       BOTTOM         |
            +----------------------+ 0
        */
        const auto bottom = rect.get_bottom(1.0);
        CHECK(is_equal(bottom, Rect::from_left_right_bottom_top(0.0, 10.0, 0.0, 1.0)));
    }
}

TEST_CASE("rect-add", "[rect]")
{
    const auto rect = Rect::from_bottom_left_size({ 0.0, 0.0 }, { 10.0, 10.0 });

    SECTION("add-left")
    {
        /*
            +---+-------------------+
            | L |   Original        |
            | E |                   |
            | F |                   |
            | T |                   |
            +---+-------------------+
            -1  0                   10
        */
        const auto left = rect.add_left(1.0);
        CHECK(is_equal(left, Rect::from_left_right_bottom_top(-1.0, 0.0, 0.0, 10.0)));
    }

    SECTION("add-right")
    {
        /*
            +--------------------+--+
            |   Original        | R |
            |                   | I |
            |                   | G |
            |                   | H |
            |                   | T |
            +-------------------+---+
            0                   10  11
        */
        const auto right = rect.add_right(1.0);
        CHECK(is_equal(right, Rect::from_left_right_bottom_top(10.0, 11.0, 0.0, 10.0)));
    }

    SECTION("add-top")
    {
        /*
            +----------------------+ 11
            |         TOP          |
            +----------------------+ 10
            |                      |
            |       Original       |
            |                      |
            +----------------------+ 0
        */
        const auto top = rect.add_top(1.0);
        CHECK(is_equal(top, Rect::from_left_right_bottom_top(0.0, 10.0, 10.0, 11.0)));
    }

    SECTION("add-bottom")
    {
        /*
            +----------------------+ 10
            |                      |
            |      Original        |
            |                      |
            +----------------------+ 0
            |       BOTTOM         |
            +----------------------+ -1
        */
        const auto bottom = rect.add_bottom(1.0);
        CHECK(is_equal(bottom, Rect::from_left_right_bottom_top(0.0, 10.0, -1.0, 0.0)));
    }
}

