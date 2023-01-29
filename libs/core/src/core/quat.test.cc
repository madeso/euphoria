#include "core/quat.h"

#include "tests/approx_equal.h"

#include "catch.hpp"


namespace euco = euphoria::core;
using namespace euphoria::tests;

TEST_CASE("quat-identity", "[quat]")
{
    const auto q = euco::quati::identity();
    REQUIRE(q == euco::quati(1, euco::vec3i(0, 0, 0)));
}

TEST_CASE("quat-testVecOp", "[quat]")
{
    REQUIRE(euco::quati(4, euco::vec3i(1, 2, 3)).get_vec_part() == euco::vec3i(1, 2, 3));
}

#define EXPECT_PRED_FORMAT2(a, b) REQUIRE(b == approx(a))

TEST_CASE("quat-testLocalAxis", "[quat]")
{
    euco::quatf q = euco::quatf::identity();
    REQUIRE(q.right() == approx(euco::unit3f::right()));
    REQUIRE(q.up() == approx(euco::unit3f::up()));
    REQUIRE(q.in() == approx(euco::unit3f::in()));

    q = euco::quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
            euco::unit3f::up(), euco::angle::from_degrees(90)));
    REQUIRE(q.right() == approx(euco::unit3f::in()));
    REQUIRE(q.up() == approx(euco::unit3f::up()));
    REQUIRE(q.in() == approx(euco::unit3f::left()));

    q = euco::quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
            euco::unit3f::right(), euco::angle::from_degrees(90)));
    REQUIRE(q.right() == approx(euco::unit3f::right()));
    REQUIRE(q.up() == approx(euco::unit3f::out()));
    REQUIRE(q.in() == approx(euco::unit3f::up()));

    q = euco::quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
            euco::unit3f::in(), euco::angle::from_degrees(90)));
    REQUIRE(q.right() == approx(euco::unit3f::down()));
    REQUIRE(q.up() == approx(euco::unit3f::right()));
    REQUIRE(q.in() == approx(euco::unit3f::in()));
}


TEST_CASE("quat-testLook", "[quat]")
{
    EXPECT_PRED_FORMAT2(
            euco::quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
                    euco::unit3f::up(), euco::angle::from_degrees(-90))),
            euco::quatf::look_at(
                    euco::vec3f(0, 0, 0),
                    euco::vec3f(5, 0, 0),
                    euco::unit3f::up()));
    EXPECT_PRED_FORMAT2(
            euco::quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
                    euco::unit3f::up(), euco::angle::from_degrees(-90))),
            euco::quatf::look_at(
                    euco::vec3f(0, 0, 0),
                    euco::vec3f(2, 0, 0),
                    euco::unit3f::up()));
    EXPECT_PRED_FORMAT2(
            euco::quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
                    euco::unit3f::up(), euco::angle::from_degrees(-90))),
            euco::quatf::look_at(
                    euco::vec3f(0, 0, 0),
                    euco::vec3f(0.25f, 0, 0),
                    euco::unit3f::up()));

    // need to pass in a normalized vec3 or it won't compile, hence the
    // .get_normalized() call
    // Z looks reversed, but remember, negative direction is in
    EXPECT_PRED_FORMAT2(
            euco::quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
                    euco::unit3f::up(), euco::angle::from_degrees(0))),
            euco::quatf::look_in_direction(
                    euco::vec3f(0, 0, -9).get_normalized(), euco::unit3f::up()));
    EXPECT_PRED_FORMAT2(
            euco::quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
                    euco::unit3f::up(), euco::angle::from_degrees(180))),
            euco::quatf::look_in_direction(
                    euco::vec3f(0, 0, 9).get_normalized(), euco::unit3f::up()));
    EXPECT_PRED_FORMAT2(
            euco::quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
                    euco::unit3f::up(), euco::angle::from_degrees(-90))),
            euco::quatf::look_in_direction(
                    euco::vec3f(3, 0, 0).get_normalized(), euco::unit3f::up()));
    EXPECT_PRED_FORMAT2(
            euco::quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
                    euco::unit3f::up(), euco::angle::from_degrees(90))),
            euco::quatf::look_in_direction(
                    euco::vec3f(-5, 0, 0).get_normalized(), euco::unit3f::up()));

    // todo(Gustav): add more test where up != up()
}


TEST_CASE("quat-testCombine", "[quat]")
{
    EXPECT_PRED_FORMAT2(
            euco::quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
                    euco::unit3f::up(), euco::angle::from_degrees(90))),
            euco::quatf::identity().rotate(
                    euco::quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
                            euco::unit3f::up(),
                            euco::angle::from_degrees(90)))));

    EXPECT_PRED_FORMAT2(
            euco::quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
                    euco::unit3f::up(), euco::angle::from_degrees(90))),
            euco::quatf::from_axis_angle(
                    euco::AxisAngle::right_hand_around(
                            euco::unit3f::up(), euco::angle::from_degrees(90)))
                    .rotate(euco::quatf::identity()));

    EXPECT_PRED_FORMAT2(
            euco::quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
                    euco::unit3f::up(), euco::angle::from_degrees(90))),
            euco::quatf::identity().rotate(
                    euco::quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
                            euco::unit3f::up(),
                            euco::angle::from_degrees(90)))));

    EXPECT_PRED_FORMAT2(

            euco::quatf::from_axis_angle(
                    euco::AxisAngle::right_hand_around(
                            euco::unit3f::up(), euco::angle::from_degrees(-90)))
                    .rotate(euco::quatf::from_axis_angle(
                            euco::AxisAngle::right_hand_around(
                                    euco::unit3f::out(),
                                    euco::angle::from_degrees(90)))),
            euco::quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
                                               euco::unit3f::right(),
                                               euco::angle::from_degrees(90)))
                    .rotate(euco::quatf::from_axis_angle(
                            euco::AxisAngle::right_hand_around(
                                    euco::unit3f::up(),
                                    euco::angle::from_degrees(-90)))));
}


TEST_CASE("quat-verifyTestAxisAngle", "[quat]")
{
    EXPECT_PRED_FORMAT2(
            euco::AxisAngle::right_hand_around(
                    euco::unit3f::up(), euco::angle::from_degrees(0)),
            euco::AxisAngle::right_hand_around(
                    euco::unit3f::up(), euco::angle::from_degrees(0)));
    EXPECT_PRED_FORMAT2(
            euco::AxisAngle::right_hand_around(
                    euco::unit3f::right(), euco::angle::from_degrees(90)),
            euco::AxisAngle::right_hand_around(
                    euco::unit3f::right(), euco::angle::from_degrees(90)));
    EXPECT_PRED_FORMAT2(
            euco::AxisAngle::right_hand_around(
                    euco::unit3f::up(), euco::angle::from_degrees(-45)),
            euco::AxisAngle::right_hand_around(
                    -euco::unit3f::up(), euco::angle::from_degrees(45)));
    EXPECT_PRED_FORMAT2(

            euco::AxisAngle::right_hand_around(
                    euco::unit3f::right(), euco::angle::from_degrees(90)),
            euco::AxisAngle::right_hand_around(
                    -euco::unit3f::right(), euco::angle::from_degrees(-90)));
}

TEST_CASE("quat-checkAxisAngle", "[quat]")
{
    EXPECT_PRED_FORMAT2(
            euco::AxisAngle::right_hand_around(
                    euco::unit3f::up(), euco::angle::from_degrees(0)),
            euco::quatf::from_axis_angle(
                    euco::AxisAngle::right_hand_around(
                            euco::unit3f::up(), euco::angle::from_degrees(0)))
                    .to_axis_angle());
    EXPECT_PRED_FORMAT2(
            euco::AxisAngle::right_hand_around(
                    euco::unit3f::right(), euco::angle::from_degrees(0)),
            euco::quatf::from_axis_angle(
                    euco::AxisAngle::right_hand_around(
                            euco::unit3f::right(), euco::angle::from_degrees(0)))
                    .to_axis_angle());
    EXPECT_PRED_FORMAT2(
            euco::AxisAngle::right_hand_around(
                    euco::unit3f::right(), euco::angle::from_degrees(90)),
            euco::quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
                                               euco::unit3f::right(),
                                               euco::angle::from_degrees(90)))
                    .to_axis_angle());
    EXPECT_PRED_FORMAT2(
            euco::AxisAngle::right_hand_around(
                    euco::unit3f::up(), euco::angle::from_degrees(-45)),
            euco::quatf::from_axis_angle(
                    euco::AxisAngle::right_hand_around(
                            -euco::unit3f::up(), euco::angle::from_degrees(45)))
                    .to_axis_angle());
    EXPECT_PRED_FORMAT2(
            euco::AxisAngle::right_hand_around(
                    euco::unit3f::right(), euco::angle::from_degrees(90)),
            euco::quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
                                               -euco::unit3f::right(),
                                               euco::angle::from_degrees(-90)))
                    .to_axis_angle());
}

TEST_CASE("quat-checkQuatConjugate", "[quat]")
{
    const auto a = euco::quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
            euco::unit3f::up(), euco::angle::from_degrees(90)));
    const auto b
            = euco::quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
                                                 euco::unit3f::up(),
                                                 euco::angle::from_degrees(-90)))
                      .get_conjugate();
    REQUIRE(b == approx(a));
}
