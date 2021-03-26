#include "core/quat.h"

#include "tests/approx_equal.h"

#include "catch.hpp"


namespace euco = euphoria::core;
using namespace euphoria::tests;

TEST_CASE("quat-identity", "[quat]")
{
    const auto q = euco::quati::Identity();
    REQUIRE(q == euco::quati(1, euco::vec3i(0, 0, 0)));
}

TEST_CASE("quat-testVecOp", "[quat]")
{
    REQUIRE(euco::quati(4, euco::vec3i(1, 2, 3)).vec() == euco::vec3i(1, 2, 3));
}

#define EXPECT_PRED_FORMAT2(a, b) REQUIRE(b == approx(a))

TEST_CASE("quat-testLocalAxis", "[quat]")
{
    euco::quatf q = euco::quatf::Identity();
    REQUIRE(q.Right() == approx(euco::unit3f::Right()));
    REQUIRE(q.Up() == approx(euco::unit3f::Up()));
    REQUIRE(q.In() == approx(euco::unit3f::In()));

    q = euco::quatf::FromAxisAngle(euco::AxisAngle::RightHandAround(
            euco::unit3f::Up(), euco::Angle::FromDegrees(90)));
    REQUIRE(q.Right() == approx(euco::unit3f::In()));
    REQUIRE(q.Up() == approx(euco::unit3f::Up()));
    REQUIRE(q.In() == approx(euco::unit3f::Left()));

    q = euco::quatf::FromAxisAngle(euco::AxisAngle::RightHandAround(
            euco::unit3f::Right(), euco::Angle::FromDegrees(90)));
    REQUIRE(q.Right() == approx(euco::unit3f::Right()));
    REQUIRE(q.Up() == approx(euco::unit3f::Out()));
    REQUIRE(q.In() == approx(euco::unit3f::Up()));

    q = euco::quatf::FromAxisAngle(euco::AxisAngle::RightHandAround(
            euco::unit3f::In(), euco::Angle::FromDegrees(90)));
    REQUIRE(q.Right() == approx(euco::unit3f::Down()));
    REQUIRE(q.Up() == approx(euco::unit3f::Right()));
    REQUIRE(q.In() == approx(euco::unit3f::In()));
}


TEST_CASE("quat-testLook", "[quat]")
{
    EXPECT_PRED_FORMAT2(
            euco::quatf::FromAxisAngle(euco::AxisAngle::RightHandAround(
                    euco::unit3f::Up(), euco::Angle::FromDegrees(-90))),
            euco::quatf::LookAt(
                    euco::vec3f(0, 0, 0),
                    euco::vec3f(5, 0, 0),
                    euco::unit3f::Up()));
    EXPECT_PRED_FORMAT2(
            euco::quatf::FromAxisAngle(euco::AxisAngle::RightHandAround(
                    euco::unit3f::Up(), euco::Angle::FromDegrees(-90))),
            euco::quatf::LookAt(
                    euco::vec3f(0, 0, 0),
                    euco::vec3f(2, 0, 0),
                    euco::unit3f::Up()));
    EXPECT_PRED_FORMAT2(
            euco::quatf::FromAxisAngle(euco::AxisAngle::RightHandAround(
                    euco::unit3f::Up(), euco::Angle::FromDegrees(-90))),
            euco::quatf::LookAt(
                    euco::vec3f(0, 0, 0),
                    euco::vec3f(0.25f, 0, 0),
                    euco::unit3f::Up()));

    // need to pass in a normalized vec3 or it won't compile, hence the
    // .GetNormalized() call
    // Z looks reversed, but remember, negative direction is in
    EXPECT_PRED_FORMAT2(
            euco::quatf::FromAxisAngle(euco::AxisAngle::RightHandAround(
                    euco::unit3f::Up(), euco::Angle::FromDegrees(0))),
            euco::quatf::LookInDirection(
                    euco::vec3f(0, 0, -9).GetNormalized(), euco::unit3f::Up()));
    EXPECT_PRED_FORMAT2(
            euco::quatf::FromAxisAngle(euco::AxisAngle::RightHandAround(
                    euco::unit3f::Up(), euco::Angle::FromDegrees(180))),
            euco::quatf::LookInDirection(
                    euco::vec3f(0, 0, 9).GetNormalized(), euco::unit3f::Up()));
    EXPECT_PRED_FORMAT2(
            euco::quatf::FromAxisAngle(euco::AxisAngle::RightHandAround(
                    euco::unit3f::Up(), euco::Angle::FromDegrees(-90))),
            euco::quatf::LookInDirection(
                    euco::vec3f(3, 0, 0).GetNormalized(), euco::unit3f::Up()));
    EXPECT_PRED_FORMAT2(
            euco::quatf::FromAxisAngle(euco::AxisAngle::RightHandAround(
                    euco::unit3f::Up(), euco::Angle::FromDegrees(90))),
            euco::quatf::LookInDirection(
                    euco::vec3f(-5, 0, 0).GetNormalized(), euco::unit3f::Up()));

    // todo: add more test where up != Up()
}


TEST_CASE("quat-testCombine", "[quat]")
{
    EXPECT_PRED_FORMAT2(
            euco::quatf::FromAxisAngle(euco::AxisAngle::RightHandAround(
                    euco::unit3f::Up(), euco::Angle::FromDegrees(90))),
            euco::quatf::Identity().Rotate(
                    euco::quatf::FromAxisAngle(euco::AxisAngle::RightHandAround(
                            euco::unit3f::Up(),
                            euco::Angle::FromDegrees(90)))));

    EXPECT_PRED_FORMAT2(
            euco::quatf::FromAxisAngle(euco::AxisAngle::RightHandAround(
                    euco::unit3f::Up(), euco::Angle::FromDegrees(90))),
            euco::quatf::FromAxisAngle(
                    euco::AxisAngle::RightHandAround(
                            euco::unit3f::Up(), euco::Angle::FromDegrees(90)))
                    .Rotate(euco::quatf::Identity()));

    EXPECT_PRED_FORMAT2(
            euco::quatf::FromAxisAngle(euco::AxisAngle::RightHandAround(
                    euco::unit3f::Up(), euco::Angle::FromDegrees(90))),
            euco::quatf::Identity().Rotate(
                    euco::quatf::FromAxisAngle(euco::AxisAngle::RightHandAround(
                            euco::unit3f::Up(),
                            euco::Angle::FromDegrees(90)))));

    EXPECT_PRED_FORMAT2(

            euco::quatf::FromAxisAngle(
                    euco::AxisAngle::RightHandAround(
                            euco::unit3f::Up(), euco::Angle::FromDegrees(-90)))
                    .Rotate(euco::quatf::FromAxisAngle(
                            euco::AxisAngle::RightHandAround(
                                    euco::unit3f::Out(),
                                    euco::Angle::FromDegrees(90)))),
            euco::quatf::FromAxisAngle(euco::AxisAngle::RightHandAround(
                                               euco::unit3f::Right(),
                                               euco::Angle::FromDegrees(90)))
                    .Rotate(euco::quatf::FromAxisAngle(
                            euco::AxisAngle::RightHandAround(
                                    euco::unit3f::Up(),
                                    euco::Angle::FromDegrees(-90)))));
}


TEST_CASE("quat-verifyTestAxisAngle", "[quat]")
{
    EXPECT_PRED_FORMAT2(
            euco::AxisAngle::RightHandAround(
                    euco::unit3f::Up(), euco::Angle::FromDegrees(0)),
            euco::AxisAngle::RightHandAround(
                    euco::unit3f::Up(), euco::Angle::FromDegrees(0)));
    EXPECT_PRED_FORMAT2(
            euco::AxisAngle::RightHandAround(
                    euco::unit3f::Right(), euco::Angle::FromDegrees(90)),
            euco::AxisAngle::RightHandAround(
                    euco::unit3f::Right(), euco::Angle::FromDegrees(90)));
    EXPECT_PRED_FORMAT2(
            euco::AxisAngle::RightHandAround(
                    euco::unit3f::Up(), euco::Angle::FromDegrees(-45)),
            euco::AxisAngle::RightHandAround(
                    -euco::unit3f::Up(), euco::Angle::FromDegrees(45)));
    EXPECT_PRED_FORMAT2(

            euco::AxisAngle::RightHandAround(
                    euco::unit3f::Right(), euco::Angle::FromDegrees(90)),
            euco::AxisAngle::RightHandAround(
                    -euco::unit3f::Right(), euco::Angle::FromDegrees(-90)));
}

TEST_CASE("quat-checkAxisAngle", "[quat]")
{
    EXPECT_PRED_FORMAT2(
            euco::AxisAngle::RightHandAround(
                    euco::unit3f::Up(), euco::Angle::FromDegrees(0)),
            euco::quatf::FromAxisAngle(
                    euco::AxisAngle::RightHandAround(
                            euco::unit3f::Up(), euco::Angle::FromDegrees(0)))
                    .ToAxisAngle());
    EXPECT_PRED_FORMAT2(
            euco::AxisAngle::RightHandAround(
                    euco::unit3f::Right(), euco::Angle::FromDegrees(0)),
            euco::quatf::FromAxisAngle(
                    euco::AxisAngle::RightHandAround(
                            euco::unit3f::Right(), euco::Angle::FromDegrees(0)))
                    .ToAxisAngle());
    EXPECT_PRED_FORMAT2(
            euco::AxisAngle::RightHandAround(
                    euco::unit3f::Right(), euco::Angle::FromDegrees(90)),
            euco::quatf::FromAxisAngle(euco::AxisAngle::RightHandAround(
                                               euco::unit3f::Right(),
                                               euco::Angle::FromDegrees(90)))
                    .ToAxisAngle());
    EXPECT_PRED_FORMAT2(
            euco::AxisAngle::RightHandAround(
                    euco::unit3f::Up(), euco::Angle::FromDegrees(-45)),
            euco::quatf::FromAxisAngle(
                    euco::AxisAngle::RightHandAround(
                            -euco::unit3f::Up(), euco::Angle::FromDegrees(45)))
                    .ToAxisAngle());
    EXPECT_PRED_FORMAT2(
            euco::AxisAngle::RightHandAround(
                    euco::unit3f::Right(), euco::Angle::FromDegrees(90)),
            euco::quatf::FromAxisAngle(euco::AxisAngle::RightHandAround(
                                               -euco::unit3f::Right(),
                                               euco::Angle::FromDegrees(-90)))
                    .ToAxisAngle());
}

TEST_CASE("quat-checkQuatConjugate", "[quat]")
{
    const auto a = euco::quatf::FromAxisAngle(euco::AxisAngle::RightHandAround(
            euco::unit3f::Up(), euco::Angle::FromDegrees(90)));
    const auto b
            = euco::quatf::FromAxisAngle(euco::AxisAngle::RightHandAround(
                                                 euco::unit3f::Up(),
                                                 euco::Angle::FromDegrees(-90)))
                      .GetConjugate();
    REQUIRE(b == approx(a));
}
