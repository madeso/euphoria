#include "base/quat.h"

#include "tests/approx_equal.h"

#include "catch.hpp"


namespace core = eu;
using namespace eu::tests;

TEST_CASE("quat-identity", "[quat]")
{
    const auto q = core::q_identity;
    REQUIRE(q == approx(core::quatf(1, core::vec3f(0, 0, 0))));
}

TEST_CASE("quat-testVecOp", "[quat]")
{
    REQUIRE(core::quatf(4, core::vec3f(1, 2, 3)).get_vec_part() == approx(core::vec3f(1, 2, 3)));
}

#define EXPECT_PRED_FORMAT2(a, b) REQUIRE(b == approx(a))

TEST_CASE("quat-testLocalAxis", "[quat]")
{
    SECTION("identity")
    {
        const auto q = core::q_identity;
        REQUIRE(q.get_right() == approx(core::common::right));
        REQUIRE(q.get_up() == approx(core::common::up));
        REQUIRE(q.get_in() == approx(core::common::in));
    }

    SECTION("look left")
    {
        const auto q = core::quatf::from_axis_angle
        (
            core::AxisAngle::from_right_hand_around
            (
                core::common::up,
                core::Angle::from_degrees(90)
            )
        );
        REQUIRE(q.get_right() == approx(core::common::in));
        REQUIRE(q.get_up() == approx(core::common::up));
        REQUIRE(q.get_in() == approx(core::common::left));
    }

    SECTION("look up")
    {
        const auto q = core::quatf::from_axis_angle
        (
            core::AxisAngle::from_right_hand_around
            (
                core::common::right,
                core::Angle::from_degrees(90)
            )
        );
        REQUIRE(q.get_right() == approx(core::common::right));
        REQUIRE(q.get_up() == approx(core::common::out));
        REQUIRE(q.get_in() == approx(core::common::up));
    }

    SECTION("roll right")
    {
        const auto q = core::quatf::from_axis_angle
        (
            core::AxisAngle::from_right_hand_around
            (
                core::common::in,
                core::Angle::from_degrees(90)
            )
        );
        REQUIRE(q.get_right() == approx(core::common::down));
        REQUIRE(q.get_up() == approx(core::common::right));
        REQUIRE(q.get_in() == approx(core::common::in));
    }
}


TEST_CASE("quat-testLook", "[quat]")
{
    EXPECT_PRED_FORMAT2
    (
        core::quatf::from_axis_angle
        (
            core::AxisAngle::from_right_hand_around
            (
                core::common::up,
                core::Angle::from_degrees(-90)
            )
        ),
        core::quatf::look_at
        (
            core::vec3f(0, 0, 0),
            core::vec3f(5, 0, 0),
            core::common::up
        )
    );
    EXPECT_PRED_FORMAT2(
            core::quatf::from_axis_angle(core::AxisAngle::from_right_hand_around(
                    core::common::up, core::Angle::from_degrees(-90))),
            core::quatf::look_at(
                    core::vec3f(0, 0, 0),
                    core::vec3f(2, 0, 0),
                    core::common::up));
    EXPECT_PRED_FORMAT2(
            core::quatf::from_axis_angle(core::AxisAngle::from_right_hand_around(
                    core::common::up, core::Angle::from_degrees(-90))),
            core::quatf::look_at(
                    core::vec3f(0, 0, 0),
                    core::vec3f(0.25f, 0, 0),
                    core::common::up));

    // need to pass in a normalized vec3 or it won't compile, hence the
    // .get_normalized() call
    // Z looks reversed, but remember, negative direction is in
    EXPECT_PRED_FORMAT2(
            core::quatf::from_axis_angle(core::AxisAngle::from_right_hand_around(
                    core::common::up, core::Angle::from_degrees(0))),
            core::quatf::look_in_direction(
                    core::vec3f(0, 0, -9).get_normalized(), core::common::up));
    EXPECT_PRED_FORMAT2(
            core::quatf::from_axis_angle(core::AxisAngle::from_right_hand_around(
                    core::common::up, core::Angle::from_degrees(180))),
            core::quatf::look_in_direction(
                    core::vec3f(0, 0, 9).get_normalized(), core::common::up));
    EXPECT_PRED_FORMAT2(
            core::quatf::from_axis_angle(core::AxisAngle::from_right_hand_around(
                    core::common::up, core::Angle::from_degrees(-90))),
            core::quatf::look_in_direction(
                    core::vec3f(3, 0, 0).get_normalized(), core::common::up));
    EXPECT_PRED_FORMAT2(
            core::quatf::from_axis_angle(core::AxisAngle::from_right_hand_around(
                    core::common::up, core::Angle::from_degrees(90))),
            core::quatf::look_in_direction(
                    core::vec3f(-5, 0, 0).get_normalized(), core::common::up));

    // todo(Gustav): add more test where up != up()
}


TEST_CASE("quat-testCombine", "[quat]")
{
    EXPECT_PRED_FORMAT2(
            core::quatf::from_axis_angle(core::AxisAngle::from_right_hand_around(
                    core::common::up, core::Angle::from_degrees(90))),
            core::q_identity.get_rotated(
                    core::quatf::from_axis_angle(core::AxisAngle::from_right_hand_around(
                            core::common::up,
                            core::Angle::from_degrees(90)))));

    EXPECT_PRED_FORMAT2(
            core::quatf::from_axis_angle(core::AxisAngle::from_right_hand_around(
                    core::common::up, core::Angle::from_degrees(90))),
            core::quatf::from_axis_angle(
                    core::AxisAngle::from_right_hand_around(
                            core::common::up, core::Angle::from_degrees(90)))
                    .get_rotated(core::q_identity));

    EXPECT_PRED_FORMAT2(
            core::quatf::from_axis_angle(core::AxisAngle::from_right_hand_around(
                    core::common::up, core::Angle::from_degrees(90))),
            core::q_identity.get_rotated(
                    core::quatf::from_axis_angle(core::AxisAngle::from_right_hand_around(
                            core::common::up,
                            core::Angle::from_degrees(90)))));

    EXPECT_PRED_FORMAT2(

            core::quatf::from_axis_angle(
                    core::AxisAngle::from_right_hand_around(
                            core::common::up, core::Angle::from_degrees(-90)))
                    .get_rotated(core::quatf::from_axis_angle(
                            core::AxisAngle::from_right_hand_around(
                                    core::common::out,
                                    core::Angle::from_degrees(90)))),
            core::quatf::from_axis_angle(core::AxisAngle::from_right_hand_around(
                                               core::common::right,
                                               core::Angle::from_degrees(90)))
                    .get_rotated(core::quatf::from_axis_angle(
                            core::AxisAngle::from_right_hand_around(
                                    core::common::up,
                                    core::Angle::from_degrees(-90)))));
}


TEST_CASE("quat-verifyTestAxisAngle", "[quat]")
{
    EXPECT_PRED_FORMAT2(
            core::AxisAngle::from_right_hand_around(
                    core::common::up, core::Angle::from_degrees(0)),
            core::AxisAngle::from_right_hand_around(
                    core::common::up, core::Angle::from_degrees(0)));
    EXPECT_PRED_FORMAT2(
            core::AxisAngle::from_right_hand_around(
                    core::common::right, core::Angle::from_degrees(90)),
            core::AxisAngle::from_right_hand_around(
                    core::common::right, core::Angle::from_degrees(90)));
    EXPECT_PRED_FORMAT2(
            core::AxisAngle::from_right_hand_around(
                    core::common::up, core::Angle::from_degrees(-45)),
            core::AxisAngle::from_right_hand_around(
                    -core::common::up, core::Angle::from_degrees(45)));
    EXPECT_PRED_FORMAT2(

            core::AxisAngle::from_right_hand_around(
                    core::common::right, core::Angle::from_degrees(90)),
            core::AxisAngle::from_right_hand_around(
                    -core::common::right, core::Angle::from_degrees(-90)));
}

TEST_CASE("quat-checkAxisAngle", "[quat]")
{
    EXPECT_PRED_FORMAT2(
            core::AxisAngle::from_right_hand_around(
                    core::common::up, core::Angle::from_degrees(0)),
            core::quatf::from_axis_angle(
                    core::AxisAngle::from_right_hand_around(
                            core::common::up, core::Angle::from_degrees(0)))
                    .to_axis_angle());
    EXPECT_PRED_FORMAT2(
            core::AxisAngle::from_right_hand_around(
                    core::common::right, core::Angle::from_degrees(0)),
            core::quatf::from_axis_angle(
                    core::AxisAngle::from_right_hand_around(
                            core::common::right, core::Angle::from_degrees(0)))
                    .to_axis_angle());
    EXPECT_PRED_FORMAT2(
            core::AxisAngle::from_right_hand_around(
                    core::common::right, core::Angle::from_degrees(90)),
            core::quatf::from_axis_angle(core::AxisAngle::from_right_hand_around(
                                               core::common::right,
                                               core::Angle::from_degrees(90)))
                    .to_axis_angle());
    EXPECT_PRED_FORMAT2(
            core::AxisAngle::from_right_hand_around(
                    core::common::up, core::Angle::from_degrees(-45)),
            core::quatf::from_axis_angle(
                    core::AxisAngle::from_right_hand_around(
                            -core::common::up, core::Angle::from_degrees(45)))
                    .to_axis_angle());
    EXPECT_PRED_FORMAT2(
            core::AxisAngle::from_right_hand_around(
                    core::common::right, core::Angle::from_degrees(90)),
            core::quatf::from_axis_angle(core::AxisAngle::from_right_hand_around(
                                               -core::common::right,
                                               core::Angle::from_degrees(-90)))
                    .to_axis_angle());
}

TEST_CASE("quat-checkQuatConjugate", "[quat]")
{
    const auto a = core::quatf::from_axis_angle(core::AxisAngle::from_right_hand_around(
            core::common::up, core::Angle::from_degrees(90)));
    const auto b
            = core::quatf::from_axis_angle(core::AxisAngle::from_right_hand_around(
                                                 core::common::up,
                                                 core::Angle::from_degrees(-90)))
                      .get_conjugate();
    REQUIRE(b == approx(a));
}
