#include "core/quat.h"

#include "tests/approx_equal.h"

#include "catch.hpp"


namespace euco = euphoria::core;
using namespace euphoria::tests;

TEST_CASE("quat-identity", "[quat]")
{
    const auto q = euco::Quati::identity();
    REQUIRE(q == euco::Quati(1, euco::Vec3i(0, 0, 0)));
}

TEST_CASE("quat-testVecOp", "[quat]")
{
    REQUIRE(euco::Quati(4, euco::Vec3i(1, 2, 3)).get_vec_part() == euco::Vec3i(1, 2, 3));
}

#define EXPECT_PRED_FORMAT2(a, b) REQUIRE(b == approx(a))

TEST_CASE("quat-testLocalAxis", "[quat]")
{
    euco::Quatf q = euco::Quatf::identity();
    REQUIRE(q.right() == approx(euco::Unit3f::right()));
    REQUIRE(q.up() == approx(euco::Unit3f::up()));
    REQUIRE(q.in() == approx(euco::Unit3f::in()));

    q = euco::Quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
            euco::Unit3f::up(), euco::Angle::from_degrees(90)));
    REQUIRE(q.right() == approx(euco::Unit3f::in()));
    REQUIRE(q.up() == approx(euco::Unit3f::up()));
    REQUIRE(q.in() == approx(euco::Unit3f::left()));

    q = euco::Quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
            euco::Unit3f::right(), euco::Angle::from_degrees(90)));
    REQUIRE(q.right() == approx(euco::Unit3f::right()));
    REQUIRE(q.up() == approx(euco::Unit3f::out()));
    REQUIRE(q.in() == approx(euco::Unit3f::up()));

    q = euco::Quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
            euco::Unit3f::in(), euco::Angle::from_degrees(90)));
    REQUIRE(q.right() == approx(euco::Unit3f::down()));
    REQUIRE(q.up() == approx(euco::Unit3f::right()));
    REQUIRE(q.in() == approx(euco::Unit3f::in()));
}


TEST_CASE("quat-testLook", "[quat]")
{
    EXPECT_PRED_FORMAT2(
            euco::Quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
                    euco::Unit3f::up(), euco::Angle::from_degrees(-90))),
            euco::Quatf::look_at(
                    euco::Vec3f(0, 0, 0),
                    euco::Vec3f(5, 0, 0),
                    euco::Unit3f::up()));
    EXPECT_PRED_FORMAT2(
            euco::Quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
                    euco::Unit3f::up(), euco::Angle::from_degrees(-90))),
            euco::Quatf::look_at(
                    euco::Vec3f(0, 0, 0),
                    euco::Vec3f(2, 0, 0),
                    euco::Unit3f::up()));
    EXPECT_PRED_FORMAT2(
            euco::Quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
                    euco::Unit3f::up(), euco::Angle::from_degrees(-90))),
            euco::Quatf::look_at(
                    euco::Vec3f(0, 0, 0),
                    euco::Vec3f(0.25f, 0, 0),
                    euco::Unit3f::up()));

    // need to pass in a normalized vec3 or it won't compile, hence the
    // .get_normalized() call
    // Z looks reversed, but remember, negative direction is in
    EXPECT_PRED_FORMAT2(
            euco::Quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
                    euco::Unit3f::up(), euco::Angle::from_degrees(0))),
            euco::Quatf::look_in_direction(
                    euco::Vec3f(0, 0, -9).get_normalized(), euco::Unit3f::up()));
    EXPECT_PRED_FORMAT2(
            euco::Quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
                    euco::Unit3f::up(), euco::Angle::from_degrees(180))),
            euco::Quatf::look_in_direction(
                    euco::Vec3f(0, 0, 9).get_normalized(), euco::Unit3f::up()));
    EXPECT_PRED_FORMAT2(
            euco::Quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
                    euco::Unit3f::up(), euco::Angle::from_degrees(-90))),
            euco::Quatf::look_in_direction(
                    euco::Vec3f(3, 0, 0).get_normalized(), euco::Unit3f::up()));
    EXPECT_PRED_FORMAT2(
            euco::Quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
                    euco::Unit3f::up(), euco::Angle::from_degrees(90))),
            euco::Quatf::look_in_direction(
                    euco::Vec3f(-5, 0, 0).get_normalized(), euco::Unit3f::up()));

    // todo(Gustav): add more test where up != up()
}


TEST_CASE("quat-testCombine", "[quat]")
{
    EXPECT_PRED_FORMAT2(
            euco::Quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
                    euco::Unit3f::up(), euco::Angle::from_degrees(90))),
            euco::Quatf::identity().rotate(
                    euco::Quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
                            euco::Unit3f::up(),
                            euco::Angle::from_degrees(90)))));

    EXPECT_PRED_FORMAT2(
            euco::Quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
                    euco::Unit3f::up(), euco::Angle::from_degrees(90))),
            euco::Quatf::from_axis_angle(
                    euco::AxisAngle::right_hand_around(
                            euco::Unit3f::up(), euco::Angle::from_degrees(90)))
                    .rotate(euco::Quatf::identity()));

    EXPECT_PRED_FORMAT2(
            euco::Quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
                    euco::Unit3f::up(), euco::Angle::from_degrees(90))),
            euco::Quatf::identity().rotate(
                    euco::Quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
                            euco::Unit3f::up(),
                            euco::Angle::from_degrees(90)))));

    EXPECT_PRED_FORMAT2(

            euco::Quatf::from_axis_angle(
                    euco::AxisAngle::right_hand_around(
                            euco::Unit3f::up(), euco::Angle::from_degrees(-90)))
                    .rotate(euco::Quatf::from_axis_angle(
                            euco::AxisAngle::right_hand_around(
                                    euco::Unit3f::out(),
                                    euco::Angle::from_degrees(90)))),
            euco::Quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
                                               euco::Unit3f::right(),
                                               euco::Angle::from_degrees(90)))
                    .rotate(euco::Quatf::from_axis_angle(
                            euco::AxisAngle::right_hand_around(
                                    euco::Unit3f::up(),
                                    euco::Angle::from_degrees(-90)))));
}


TEST_CASE("quat-verifyTestAxisAngle", "[quat]")
{
    EXPECT_PRED_FORMAT2(
            euco::AxisAngle::right_hand_around(
                    euco::Unit3f::up(), euco::Angle::from_degrees(0)),
            euco::AxisAngle::right_hand_around(
                    euco::Unit3f::up(), euco::Angle::from_degrees(0)));
    EXPECT_PRED_FORMAT2(
            euco::AxisAngle::right_hand_around(
                    euco::Unit3f::right(), euco::Angle::from_degrees(90)),
            euco::AxisAngle::right_hand_around(
                    euco::Unit3f::right(), euco::Angle::from_degrees(90)));
    EXPECT_PRED_FORMAT2(
            euco::AxisAngle::right_hand_around(
                    euco::Unit3f::up(), euco::Angle::from_degrees(-45)),
            euco::AxisAngle::right_hand_around(
                    -euco::Unit3f::up(), euco::Angle::from_degrees(45)));
    EXPECT_PRED_FORMAT2(

            euco::AxisAngle::right_hand_around(
                    euco::Unit3f::right(), euco::Angle::from_degrees(90)),
            euco::AxisAngle::right_hand_around(
                    -euco::Unit3f::right(), euco::Angle::from_degrees(-90)));
}

TEST_CASE("quat-checkAxisAngle", "[quat]")
{
    EXPECT_PRED_FORMAT2(
            euco::AxisAngle::right_hand_around(
                    euco::Unit3f::up(), euco::Angle::from_degrees(0)),
            euco::Quatf::from_axis_angle(
                    euco::AxisAngle::right_hand_around(
                            euco::Unit3f::up(), euco::Angle::from_degrees(0)))
                    .to_axis_angle());
    EXPECT_PRED_FORMAT2(
            euco::AxisAngle::right_hand_around(
                    euco::Unit3f::right(), euco::Angle::from_degrees(0)),
            euco::Quatf::from_axis_angle(
                    euco::AxisAngle::right_hand_around(
                            euco::Unit3f::right(), euco::Angle::from_degrees(0)))
                    .to_axis_angle());
    EXPECT_PRED_FORMAT2(
            euco::AxisAngle::right_hand_around(
                    euco::Unit3f::right(), euco::Angle::from_degrees(90)),
            euco::Quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
                                               euco::Unit3f::right(),
                                               euco::Angle::from_degrees(90)))
                    .to_axis_angle());
    EXPECT_PRED_FORMAT2(
            euco::AxisAngle::right_hand_around(
                    euco::Unit3f::up(), euco::Angle::from_degrees(-45)),
            euco::Quatf::from_axis_angle(
                    euco::AxisAngle::right_hand_around(
                            -euco::Unit3f::up(), euco::Angle::from_degrees(45)))
                    .to_axis_angle());
    EXPECT_PRED_FORMAT2(
            euco::AxisAngle::right_hand_around(
                    euco::Unit3f::right(), euco::Angle::from_degrees(90)),
            euco::Quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
                                               -euco::Unit3f::right(),
                                               euco::Angle::from_degrees(-90)))
                    .to_axis_angle());
}

TEST_CASE("quat-checkQuatConjugate", "[quat]")
{
    const auto a = euco::Quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
            euco::Unit3f::up(), euco::Angle::from_degrees(90)));
    const auto b
            = euco::Quatf::from_axis_angle(euco::AxisAngle::right_hand_around(
                                                 euco::Unit3f::up(),
                                                 euco::Angle::from_degrees(-90)))
                      .get_conjugate();
    REQUIRE(b == approx(a));
}
