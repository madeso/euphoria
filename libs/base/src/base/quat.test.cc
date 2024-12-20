#include "base/quat.h"

#include "tests/approx_equal.h"

#include "catch2/catch_all.hpp"



using namespace eu::tests;

TEST_CASE("quat-identity", "[quat]")
{
    const auto q = eu::q_identity;
    REQUIRE(q == approx(eu::quatf(1, eu::v3(0, 0, 0))));
}

TEST_CASE("quat-testVecOp", "[quat]")
{
    REQUIRE(eu::quatf(4, eu::v3(1, 2, 3)).get_vec_part() == approx(eu::v3(1, 2, 3)));
}

#define EXPECT_PRED_FORMAT2(a, b) REQUIRE(b == approx(a))

TEST_CASE("quat-testLocalAxis", "[quat]")
{
    SECTION("identity")
    {
        const auto q = eu::q_identity;
        REQUIRE(q.get_right() == approx(eu::common::right));
        REQUIRE(q.get_up() == approx(eu::common::up));
        REQUIRE(q.get_in() == approx(eu::common::in));
    }

    SECTION("look left")
    {
        const auto q = eu::quatf::from_axis_angle
        (
            eu::AxisAngle::from_right_hand_around
            (
                eu::common::up,
                eu::Angle::from_degrees(90)
            )
        );
        REQUIRE(q.get_right() == approx(eu::common::in));
        REQUIRE(q.get_up() == approx(eu::common::up));
        REQUIRE(q.get_in() == approx(eu::common::left));
    }

    SECTION("look up")
    {
        const auto q = eu::quatf::from_axis_angle
        (
            eu::AxisAngle::from_right_hand_around
            (
                eu::common::right,
                eu::Angle::from_degrees(90)
            )
        );
        REQUIRE(q.get_right() == approx(eu::common::right));
        REQUIRE(q.get_up() == approx(eu::common::out));
        REQUIRE(q.get_in() == approx(eu::common::up));
    }

    SECTION("roll right")
    {
        const auto q = eu::quatf::from_axis_angle
        (
            eu::AxisAngle::from_right_hand_around
            (
                eu::common::in,
                eu::Angle::from_degrees(90)
            )
        );
        REQUIRE(q.get_right() == approx(eu::common::down));
        REQUIRE(q.get_up() == approx(eu::common::right));
        REQUIRE(q.get_in() == approx(eu::common::in));
    }
}


TEST_CASE("quat-testLook", "[quat]")
{
    EXPECT_PRED_FORMAT2
    (
        eu::quatf::from_axis_angle
        (
            eu::AxisAngle::from_right_hand_around
            (
                eu::common::up,
                eu::Angle::from_degrees(-90)
            )
        ),
        eu::quatf::look_at
        (
            eu::v3(0, 0, 0),
            eu::v3(5, 0, 0),
            eu::common::up
        )
    );
    EXPECT_PRED_FORMAT2(
            eu::quatf::from_axis_angle(eu::AxisAngle::from_right_hand_around(
                    eu::common::up, eu::Angle::from_degrees(-90))),
            eu::quatf::look_at(
                    eu::v3(0, 0, 0),
                    eu::v3(2, 0, 0),
                    eu::common::up));
    EXPECT_PRED_FORMAT2(
            eu::quatf::from_axis_angle(eu::AxisAngle::from_right_hand_around(
                    eu::common::up, eu::Angle::from_degrees(-90))),
            eu::quatf::look_at(
                    eu::v3(0, 0, 0),
                    eu::v3(0.25f, 0, 0),
                    eu::common::up));

    // need to pass in a normalized vec3 or it won't compile, hence the
    // .get_normalized() call
    // Z looks reversed, but remember, negative direction is in
    EXPECT_PRED_FORMAT2(
            eu::quatf::from_axis_angle(eu::AxisAngle::from_right_hand_around(
                    eu::common::up, eu::Angle::from_degrees(0))),
            eu::quatf::look_in_direction(
                    eu::v3(0, 0, -9).get_normalized(), eu::common::up));
    EXPECT_PRED_FORMAT2(
            eu::quatf::from_axis_angle(eu::AxisAngle::from_right_hand_around(
                    eu::common::up, eu::Angle::from_degrees(180))),
            eu::quatf::look_in_direction(
                    eu::v3(0, 0, 9).get_normalized(), eu::common::up));
    EXPECT_PRED_FORMAT2(
            eu::quatf::from_axis_angle(eu::AxisAngle::from_right_hand_around(
                    eu::common::up, eu::Angle::from_degrees(-90))),
            eu::quatf::look_in_direction(
                    eu::v3(3, 0, 0).get_normalized(), eu::common::up));
    EXPECT_PRED_FORMAT2(
            eu::quatf::from_axis_angle(eu::AxisAngle::from_right_hand_around(
                    eu::common::up, eu::Angle::from_degrees(90))),
            eu::quatf::look_in_direction(
                    eu::v3(-5, 0, 0).get_normalized(), eu::common::up));

    // todo(Gustav): add more test where up != up()
}


TEST_CASE("quat-testCombine", "[quat]")
{
    EXPECT_PRED_FORMAT2(
            eu::quatf::from_axis_angle(eu::AxisAngle::from_right_hand_around(
                    eu::common::up, eu::Angle::from_degrees(90))),
            eu::q_identity.get_rotated(
                    eu::quatf::from_axis_angle(eu::AxisAngle::from_right_hand_around(
                            eu::common::up,
                            eu::Angle::from_degrees(90)))));

    EXPECT_PRED_FORMAT2(
            eu::quatf::from_axis_angle(eu::AxisAngle::from_right_hand_around(
                    eu::common::up, eu::Angle::from_degrees(90))),
            eu::quatf::from_axis_angle(
                    eu::AxisAngle::from_right_hand_around(
                            eu::common::up, eu::Angle::from_degrees(90)))
                    .get_rotated(eu::q_identity));

    EXPECT_PRED_FORMAT2(
            eu::quatf::from_axis_angle(eu::AxisAngle::from_right_hand_around(
                    eu::common::up, eu::Angle::from_degrees(90))),
            eu::q_identity.get_rotated(
                    eu::quatf::from_axis_angle(eu::AxisAngle::from_right_hand_around(
                            eu::common::up,
                            eu::Angle::from_degrees(90)))));

    EXPECT_PRED_FORMAT2(

            eu::quatf::from_axis_angle(
                    eu::AxisAngle::from_right_hand_around(
                            eu::common::up, eu::Angle::from_degrees(-90)))
                    .get_rotated(eu::quatf::from_axis_angle(
                            eu::AxisAngle::from_right_hand_around(
                                    eu::common::out,
                                    eu::Angle::from_degrees(90)))),
            eu::quatf::from_axis_angle(eu::AxisAngle::from_right_hand_around(
                                               eu::common::right,
                                               eu::Angle::from_degrees(90)))
                    .get_rotated(eu::quatf::from_axis_angle(
                            eu::AxisAngle::from_right_hand_around(
                                    eu::common::up,
                                    eu::Angle::from_degrees(-90)))));
}


TEST_CASE("quat-verifyTestAxisAngle", "[quat]")
{
    EXPECT_PRED_FORMAT2(
            eu::AxisAngle::from_right_hand_around(
                    eu::common::up, eu::Angle::from_degrees(0)),
            eu::AxisAngle::from_right_hand_around(
                    eu::common::up, eu::Angle::from_degrees(0)));
    EXPECT_PRED_FORMAT2(
            eu::AxisAngle::from_right_hand_around(
                    eu::common::right, eu::Angle::from_degrees(90)),
            eu::AxisAngle::from_right_hand_around(
                    eu::common::right, eu::Angle::from_degrees(90)));
    EXPECT_PRED_FORMAT2(
            eu::AxisAngle::from_right_hand_around(
                    eu::common::up, eu::Angle::from_degrees(-45)),
            eu::AxisAngle::from_right_hand_around(
                    -eu::common::up, eu::Angle::from_degrees(45)));
    EXPECT_PRED_FORMAT2(

            eu::AxisAngle::from_right_hand_around(
                    eu::common::right, eu::Angle::from_degrees(90)),
            eu::AxisAngle::from_right_hand_around(
                    -eu::common::right, eu::Angle::from_degrees(-90)));
}

TEST_CASE("quat-checkAxisAngle", "[quat]")
{
    EXPECT_PRED_FORMAT2(
            eu::AxisAngle::from_right_hand_around(
                    eu::common::up, eu::Angle::from_degrees(0)),
            eu::quatf::from_axis_angle(
                    eu::AxisAngle::from_right_hand_around(
                            eu::common::up, eu::Angle::from_degrees(0)))
                    .to_axis_angle());
    EXPECT_PRED_FORMAT2(
            eu::AxisAngle::from_right_hand_around(
                    eu::common::right, eu::Angle::from_degrees(0)),
            eu::quatf::from_axis_angle(
                    eu::AxisAngle::from_right_hand_around(
                            eu::common::right, eu::Angle::from_degrees(0)))
                    .to_axis_angle());
    EXPECT_PRED_FORMAT2(
            eu::AxisAngle::from_right_hand_around(
                    eu::common::right, eu::Angle::from_degrees(90)),
            eu::quatf::from_axis_angle(eu::AxisAngle::from_right_hand_around(
                                               eu::common::right,
                                               eu::Angle::from_degrees(90)))
                    .to_axis_angle());
    EXPECT_PRED_FORMAT2(
            eu::AxisAngle::from_right_hand_around(
                    eu::common::up, eu::Angle::from_degrees(-45)),
            eu::quatf::from_axis_angle(
                    eu::AxisAngle::from_right_hand_around(
                            -eu::common::up, eu::Angle::from_degrees(45)))
                    .to_axis_angle());
    EXPECT_PRED_FORMAT2(
            eu::AxisAngle::from_right_hand_around(
                    eu::common::right, eu::Angle::from_degrees(90)),
            eu::quatf::from_axis_angle(eu::AxisAngle::from_right_hand_around(
                                               -eu::common::right,
                                               eu::Angle::from_degrees(-90)))
                    .to_axis_angle());
}

TEST_CASE("quat-checkQuatConjugate", "[quat]")
{
    const auto a = eu::quatf::from_axis_angle(eu::AxisAngle::from_right_hand_around(
            eu::common::up, eu::Angle::from_degrees(90)));
    const auto b
            = eu::quatf::from_axis_angle(eu::AxisAngle::from_right_hand_around(
                                                 eu::common::up,
                                                 eu::Angle::from_degrees(-90)))
                      .get_conjugate();
    REQUIRE(b == approx(a));
}
