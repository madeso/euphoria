#include "core/quat.h"

#include "tests/approx_equal.h"

#include "catch.hpp"


namespace euco = euphoria::core;
using namespace euphoria::tests;

TEST_CASE("quat-identity", "[quat]")
{
    const auto q = euco::q_identity;
    REQUIRE(q == approx(euco::quatf(1, euco::vec3f(0, 0, 0))));
}

TEST_CASE("quat-testVecOp", "[quat]")
{
    REQUIRE(euco::quatf(4, euco::vec3f(1, 2, 3)).get_vec_part() == approx(euco::vec3f(1, 2, 3)));
}

#define EXPECT_PRED_FORMAT2(a, b) REQUIRE(b == approx(a))

TEST_CASE("quat-testLocalAxis", "[quat]")
{
    SECTION("identity")
    {
        const auto q = euco::q_identity;
        REQUIRE(q.get_right() == approx(euco::common::right));
        REQUIRE(q.get_up() == approx(euco::common::up));
        REQUIRE(q.get_in() == approx(euco::common::in));
    }

    SECTION("look left")
    {
        const auto q = euco::quatf::from_axis_angle
        (
            euco::AxisAngle::from_right_hand_around
            (
                euco::common::up,
                euco::Angle::from_degrees(90)
            )
        );
        REQUIRE(q.get_right() == approx(euco::common::in));
        REQUIRE(q.get_up() == approx(euco::common::up));
        REQUIRE(q.get_in() == approx(euco::common::left));
    }

    SECTION("look up")
    {
        const auto q = euco::quatf::from_axis_angle
        (
            euco::AxisAngle::from_right_hand_around
            (
                euco::common::right,
                euco::Angle::from_degrees(90)
            )
        );
        REQUIRE(q.get_right() == approx(euco::common::right));
        REQUIRE(q.get_up() == approx(euco::common::out));
        REQUIRE(q.get_in() == approx(euco::common::up));
    }

    SECTION("roll right")
    {
        const auto q = euco::quatf::from_axis_angle
        (
            euco::AxisAngle::from_right_hand_around
            (
                euco::common::in,
                euco::Angle::from_degrees(90)
            )
        );
        REQUIRE(q.get_right() == approx(euco::common::down));
        REQUIRE(q.get_up() == approx(euco::common::right));
        REQUIRE(q.get_in() == approx(euco::common::in));
    }
}


TEST_CASE("quat-testLook", "[quat]")
{
    EXPECT_PRED_FORMAT2
    (
        euco::quatf::from_axis_angle
        (
            euco::AxisAngle::from_right_hand_around
            (
                euco::common::up,
                euco::Angle::from_degrees(-90)
            )
        ),
        euco::quatf::look_at
        (
            euco::vec3f(0, 0, 0),
            euco::vec3f(5, 0, 0),
            euco::common::up
        )
    );
    EXPECT_PRED_FORMAT2(
            euco::quatf::from_axis_angle(euco::AxisAngle::from_right_hand_around(
                    euco::common::up, euco::Angle::from_degrees(-90))),
            euco::quatf::look_at(
                    euco::vec3f(0, 0, 0),
                    euco::vec3f(2, 0, 0),
                    euco::common::up));
    EXPECT_PRED_FORMAT2(
            euco::quatf::from_axis_angle(euco::AxisAngle::from_right_hand_around(
                    euco::common::up, euco::Angle::from_degrees(-90))),
            euco::quatf::look_at(
                    euco::vec3f(0, 0, 0),
                    euco::vec3f(0.25f, 0, 0),
                    euco::common::up));

    // need to pass in a normalized vec3 or it won't compile, hence the
    // .get_normalized() call
    // Z looks reversed, but remember, negative direction is in
    EXPECT_PRED_FORMAT2(
            euco::quatf::from_axis_angle(euco::AxisAngle::from_right_hand_around(
                    euco::common::up, euco::Angle::from_degrees(0))),
            euco::quatf::look_in_direction(
                    euco::vec3f(0, 0, -9).get_normalized(), euco::common::up));
    EXPECT_PRED_FORMAT2(
            euco::quatf::from_axis_angle(euco::AxisAngle::from_right_hand_around(
                    euco::common::up, euco::Angle::from_degrees(180))),
            euco::quatf::look_in_direction(
                    euco::vec3f(0, 0, 9).get_normalized(), euco::common::up));
    EXPECT_PRED_FORMAT2(
            euco::quatf::from_axis_angle(euco::AxisAngle::from_right_hand_around(
                    euco::common::up, euco::Angle::from_degrees(-90))),
            euco::quatf::look_in_direction(
                    euco::vec3f(3, 0, 0).get_normalized(), euco::common::up));
    EXPECT_PRED_FORMAT2(
            euco::quatf::from_axis_angle(euco::AxisAngle::from_right_hand_around(
                    euco::common::up, euco::Angle::from_degrees(90))),
            euco::quatf::look_in_direction(
                    euco::vec3f(-5, 0, 0).get_normalized(), euco::common::up));

    // todo(Gustav): add more test where up != up()
}


TEST_CASE("quat-testCombine", "[quat]")
{
    EXPECT_PRED_FORMAT2(
            euco::quatf::from_axis_angle(euco::AxisAngle::from_right_hand_around(
                    euco::common::up, euco::Angle::from_degrees(90))),
            euco::q_identity.get_rotated(
                    euco::quatf::from_axis_angle(euco::AxisAngle::from_right_hand_around(
                            euco::common::up,
                            euco::Angle::from_degrees(90)))));

    EXPECT_PRED_FORMAT2(
            euco::quatf::from_axis_angle(euco::AxisAngle::from_right_hand_around(
                    euco::common::up, euco::Angle::from_degrees(90))),
            euco::quatf::from_axis_angle(
                    euco::AxisAngle::from_right_hand_around(
                            euco::common::up, euco::Angle::from_degrees(90)))
                    .get_rotated(euco::q_identity));

    EXPECT_PRED_FORMAT2(
            euco::quatf::from_axis_angle(euco::AxisAngle::from_right_hand_around(
                    euco::common::up, euco::Angle::from_degrees(90))),
            euco::q_identity.get_rotated(
                    euco::quatf::from_axis_angle(euco::AxisAngle::from_right_hand_around(
                            euco::common::up,
                            euco::Angle::from_degrees(90)))));

    EXPECT_PRED_FORMAT2(

            euco::quatf::from_axis_angle(
                    euco::AxisAngle::from_right_hand_around(
                            euco::common::up, euco::Angle::from_degrees(-90)))
                    .get_rotated(euco::quatf::from_axis_angle(
                            euco::AxisAngle::from_right_hand_around(
                                    euco::common::out,
                                    euco::Angle::from_degrees(90)))),
            euco::quatf::from_axis_angle(euco::AxisAngle::from_right_hand_around(
                                               euco::common::right,
                                               euco::Angle::from_degrees(90)))
                    .get_rotated(euco::quatf::from_axis_angle(
                            euco::AxisAngle::from_right_hand_around(
                                    euco::common::up,
                                    euco::Angle::from_degrees(-90)))));
}


TEST_CASE("quat-verifyTestAxisAngle", "[quat]")
{
    EXPECT_PRED_FORMAT2(
            euco::AxisAngle::from_right_hand_around(
                    euco::common::up, euco::Angle::from_degrees(0)),
            euco::AxisAngle::from_right_hand_around(
                    euco::common::up, euco::Angle::from_degrees(0)));
    EXPECT_PRED_FORMAT2(
            euco::AxisAngle::from_right_hand_around(
                    euco::common::right, euco::Angle::from_degrees(90)),
            euco::AxisAngle::from_right_hand_around(
                    euco::common::right, euco::Angle::from_degrees(90)));
    EXPECT_PRED_FORMAT2(
            euco::AxisAngle::from_right_hand_around(
                    euco::common::up, euco::Angle::from_degrees(-45)),
            euco::AxisAngle::from_right_hand_around(
                    -euco::common::up, euco::Angle::from_degrees(45)));
    EXPECT_PRED_FORMAT2(

            euco::AxisAngle::from_right_hand_around(
                    euco::common::right, euco::Angle::from_degrees(90)),
            euco::AxisAngle::from_right_hand_around(
                    -euco::common::right, euco::Angle::from_degrees(-90)));
}

TEST_CASE("quat-checkAxisAngle", "[quat]")
{
    EXPECT_PRED_FORMAT2(
            euco::AxisAngle::from_right_hand_around(
                    euco::common::up, euco::Angle::from_degrees(0)),
            euco::quatf::from_axis_angle(
                    euco::AxisAngle::from_right_hand_around(
                            euco::common::up, euco::Angle::from_degrees(0)))
                    .to_axis_angle());
    EXPECT_PRED_FORMAT2(
            euco::AxisAngle::from_right_hand_around(
                    euco::common::right, euco::Angle::from_degrees(0)),
            euco::quatf::from_axis_angle(
                    euco::AxisAngle::from_right_hand_around(
                            euco::common::right, euco::Angle::from_degrees(0)))
                    .to_axis_angle());
    EXPECT_PRED_FORMAT2(
            euco::AxisAngle::from_right_hand_around(
                    euco::common::right, euco::Angle::from_degrees(90)),
            euco::quatf::from_axis_angle(euco::AxisAngle::from_right_hand_around(
                                               euco::common::right,
                                               euco::Angle::from_degrees(90)))
                    .to_axis_angle());
    EXPECT_PRED_FORMAT2(
            euco::AxisAngle::from_right_hand_around(
                    euco::common::up, euco::Angle::from_degrees(-45)),
            euco::quatf::from_axis_angle(
                    euco::AxisAngle::from_right_hand_around(
                            -euco::common::up, euco::Angle::from_degrees(45)))
                    .to_axis_angle());
    EXPECT_PRED_FORMAT2(
            euco::AxisAngle::from_right_hand_around(
                    euco::common::right, euco::Angle::from_degrees(90)),
            euco::quatf::from_axis_angle(euco::AxisAngle::from_right_hand_around(
                                               -euco::common::right,
                                               euco::Angle::from_degrees(-90)))
                    .to_axis_angle());
}

TEST_CASE("quat-checkQuatConjugate", "[quat]")
{
    const auto a = euco::quatf::from_axis_angle(euco::AxisAngle::from_right_hand_around(
            euco::common::up, euco::Angle::from_degrees(90)));
    const auto b
            = euco::quatf::from_axis_angle(euco::AxisAngle::from_right_hand_around(
                                                 euco::common::up,
                                                 euco::Angle::from_degrees(-90)))
                      .get_conjugate();
    REQUIRE(b == approx(a));
}
