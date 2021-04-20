#include "core/mat4.h"
#include "core/numeric.h"

#include "tests/approx_equal.h"

#include "catch.hpp"

namespace euco = euphoria::core;
using namespace euphoria::tests;

TEST_CASE("mat4-colmajor", "[mat]")
{
    const auto m = euco::mat4i::from_col_major(
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
    REQUIRE(euco::vec4i(0, 1, 2, 3) == m.get_column(0));
    REQUIRE(euco::vec4i(4, 5, 6, 7) == m.get_column(1));
    REQUIRE(euco::vec4i(8, 9, 10, 11) == m.get_column(2));
    REQUIRE(euco::vec4i(12, 13, 14, 15) == m.get_column(3));
}

TEST_CASE("mat4-rowmajor", "[mat]")
{
    const auto m = euco::mat4i::from_row_major(
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
    REQUIRE(euco::vec4i(0, 4, 8, 12) == m.get_column(0));
    REQUIRE(euco::vec4i(1, 5, 9, 13) == m.get_column(1));
    REQUIRE(euco::vec4i(2, 6, 10, 14) == m.get_column(2));
    REQUIRE(euco::vec4i(3, 7, 11, 15) == m.get_column(3));
}

TEST_CASE("mat4-rowmajor_row", "[mat]")
{
    const auto m = euco::mat4i::from_row_major(
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
    REQUIRE(euco::vec4i(0, 1, 2, 3) == m.get_row(0));
    REQUIRE(euco::vec4i(4, 5, 6, 7) == m.get_row(1));
    REQUIRE(euco::vec4i(8, 9, 10, 11) == m.get_row(2));
    REQUIRE(euco::vec4i(12, 13, 14, 15) == m.get_row(3));
}

TEST_CASE("mat4-identity", "[mat]")
{
    const auto m = euco::mat4i::from_row_major(
            1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
    const auto i = euco::mat4i::identity();
    REQUIRE(i.get_column(0) == m.get_column(0));
    REQUIRE(i.get_column(1) == m.get_column(1));
    REQUIRE(i.get_column(2) == m.get_column(2));
    REQUIRE(i.get_column(3) == m.get_column(3));
}

TEST_CASE("mat4-index", "[mat]")
{
    const auto m = euco::mat4i::from_row_major(
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
    REQUIRE(3 == m(0, 3));
    REQUIRE(15 == m(3, 3));
}

TEST_CASE("mat4-major", "[mat]")
{
    const auto m = euco::mat4i::from_row_major(
                           0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15)
                           .get_major();
    REQUIRE(euco::vec4i(0, 5, 10, 15) == m);
}


TEST_CASE("mat4-transposed", "[mat]")
{
    const auto m = euco::mat4i::from_row_major(
                           0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15)
                           .get_transposed();
    REQUIRE(m
            == euco::mat4i::from_col_major(
                    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15));
}

TEST_CASE("mat4-axis", "[mat]")
{
    const auto m = euco::mat4i::identity();
    REQUIRE(euco::vec3i(1, 0, 0) == static_cast<euco::vec3i>(m.get_x_axis()));
    REQUIRE(euco::vec3i(0, 1, 0) == m.get_y_axis());
    REQUIRE(euco::vec3i(0, 0, 1) == m.get_z_axis());
}

TEST_CASE("mat4-multiply", "[mat]")
{
    const auto m = euco::mat4i::from_row_major(
                           0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15)
                   * euco::mat4i::from_row_major(
                           16,
                           17,
                           18,
                           19,
                           20,
                           21,
                           22,
                           23,
                           24,
                           25,
                           26,
                           27,
                           28,
                           29,
                           30,
                           31);
    // thanks wolfram alpha for the result
    // http://www.wolframalpha.com/widgets/view.jsp?id=cc71c2e95a80c217564d530fd8297b0e
    // simplify({{0,1,2,3},{4,5,6,7},{8,9,10,11},{12,13,14,15}}.{{16,17,18,19},{20,21,22,23},{24,25,26,27},{28,29,30,31}})
    REQUIRE(m
            == euco::mat4i::from_row_major(
                    152,
                    158,
                    164,
                    170,
                    504,
                    526,
                    548,
                    570,
                    856,
                    894,
                    932,
                    970,
                    1208,
                    1262,
                    1316,
                    1370));
}

TEST_CASE("mat4-add", "[mat]")
{
    const auto m = euco::mat4i::from_row_major(
                           0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15)
                   + euco::mat4i::from_row_major(
                           16,
                           17,
                           18,
                           19,
                           20,
                           21,
                           22,
                           23,
                           24,
                           25,
                           26,
                           27,
                           28,
                           29,
                           30,
                           31);
    REQUIRE(m
            == euco::mat4i::from_row_major(
                    16,
                    18,
                    20,
                    22,
                    24,
                    26,
                    28,
                    30,
                    32,
                    34,
                    36,
                    38,
                    40,
                    42,
                    44,
                    46));
}

TEST_CASE("mat4-sub", "[mat]")
{
    const auto m
            = euco::mat4i::from_row_major(
                      16,
                      17,
                      18,
                      19,
                      20,
                      21,
                      22,
                      23,
                      24,
                      25,
                      26,
                      27,
                      28,
                      29,
                      30,
                      31)
              - euco::mat4i::from_row_major(
                      0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
    REQUIRE(m
            == euco::mat4i::from_row_major(
                    16,
                    16,
                    16,
                    16,
                    16,
                    16,
                    16,
                    16,
                    16,
                    16,
                    16,
                    16,
                    16,
                    16,
                    16,
                    16));
}


TEST_CASE("mat4-mat3", "[mat]")
{
    const auto m4 = euco::mat4i::from_col_major(
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
    const auto m3 = m4.get_mat3();
    REQUIRE(std::make_tuple(0, 1, 2) == m3.get_column(0));
    REQUIRE(std::make_tuple(4, 5, 6) == m3.get_column(1));
    REQUIRE(std::make_tuple(8, 9, 10) == m3.get_column(2));
}


TEST_CASE("mat4-inverse", "[mat]")
{
    const auto Mat0 = euco::mat4f::from_col_major(
            0.6f,
            0.2f,
            0.3f,
            0.4f,
            0.2f,
            0.7f,
            0.5f,
            0.3f,
            0.3f,
            0.5f,
            0.7f,
            0.2f,
            0.4f,
            0.3f,
            0.2f,
            0.6f);

    auto Inv0 = Mat0;  // glm::inverse(Mat0);
    REQUIRE(Inv0.invert());
    REQUIRE(Inv0.invert());

    // EXPECT_EQ(Mat0, Inv0);
}


////////////////////////////////////////////////////////////////////////////////

TEST_CASE("mat4-TestTransformation", "[mat]")
{
    REQUIRE(euco::vec3i(1, 2, 3)
            == euco::mat4i::from_translation(euco::vec3i(1, 2, 3))
                       .get_translation());
}


TEST_CASE("mat4-TestRight", "[mat]")
{
    REQUIRE(euco::unit3i::XAxis() == euco::mat4i::identity().get_x_axis());
}


TEST_CASE("mat4-TestUp", "[mat]")
{
    REQUIRE(euco::unit3i::YAxis() == euco::mat4i::identity().get_y_axis());
}


TEST_CASE("mat4-TestIn", "[mat]")
{
    REQUIRE(euco::unit3i::ZAxis() == euco::mat4i::identity().get_z_axis());
}

TEST_CASE("mat4-test", "[mat]")
{
    const euco::mat4f start = euco::mat4f::identity();
    euco::axis_angle   aa    = euco::axis_angle::right_hand_around(
            euco::unit3f::Up(), euco::angle::from_degrees(-90));
    const euco::vec3f toTransform(0, 0, -5);
    const euco::vec3f result(5, 0, 0);

    SECTION("TestRotationAxisAngle")
    {
        const auto r = start.rotate(aa).get_transform_point(toTransform);
        REQUIRE(r == approx(result));
    }
}


TEST_CASE("mat4-TestCombined_RT", "[mat]")
{
    const auto r = euco::mat4f::identity()
                           .rotate(euco::axis_angle::right_hand_around(
                                   euco::unit3f::Up(),
                                   euco::angle::from_degrees(-90)))
                           .translate(euco::vec3f(0, 0, -5))
                           .get_transform_point(euco::vec3f(0, 0, 0));
    REQUIRE(r == approx(euco::vec3f(5, 0, 0)));
}

TEST_CASE("mat4-TestCombined2_RT", "[mat]")
{
    const auto r
            = euco::mat4f::identity()
                      .rotate(euco::axis_angle::right_hand_around(
                              euco::unit3f::Up(), euco::angle::from_degrees(90)))
                      .translate(euco::vec3f(0, 0, -5))
                      .get_transform_point(euco::vec3f(0, 0, 0));
    REQUIRE(r == approx(euco::vec3f(-5, 0, 0)));
}

TEST_CASE("mat4-TestCombined_TR", "[mat]")
{
    const auto r = euco::mat4f::identity()
                           .translate(euco::vec3f(0, 0, 5))
                           .rotate(euco::axis_angle::right_hand_around(
                                   euco::unit3f::Up(),
                                   euco::angle::from_degrees(-90)))
                           .get_transform_point(euco::vec3f(0, 0, 0));
    REQUIRE(r == approx(euco::vec3f(0, 0, 5)));
}

TEST_CASE("mat4-TestTranslation", "[mat]")
{
    const auto r = euco::mat4i::identity()
                           .translate(euco::vec3i(1, 2, 3))
                           .get_transform_point(euco::vec3i(7, 8, 9));
    REQUIRE(r == euco::vec3i(8, 10, 12));
}


TEST_CASE("mat4-TestIentityTransform", "[mat]")
{
    const auto r
            = euco::mat4i::identity().get_transform_point(euco::vec3i(1, 2, 3));
    REQUIRE(r == euco::vec3i(1, 2, 3));
}

TEST_CASE("mat4-TestIentityMultiply", "[mat]")
{
    REQUIRE(euco::mat4i::identity() * euco::mat4i::identity()
            == euco::mat4i::identity());
}

TEST_CASE("mat4-TestVec4Multiply", "[mat]")
{
    const auto m = euco::mat4i::from_row_major(
                           0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15)
                   * euco::vec4i(16, 17, 18, 19);
    // simplify({{0,1,2,3},{4,5,6,7},{8,9,10,11},{12,13,14,15}}.{16,17,18,19})
    REQUIRE(m == euco::vec4i(110, 390, 670, 950));
}
