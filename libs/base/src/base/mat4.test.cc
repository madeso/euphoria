#include "base/mat4.h"
#include "base/numeric.h"

#include "tests/approx_equal.h"

#include "catch2/catch_all.hpp"


using namespace eu::tests;

TEST_CASE("mat4-colmajor", "[mat]")
{
    const auto m = eu::m4::from_col_major
    (
         0.0f,   1.0f,   2.0f,   3.0f,
         4.0f,   5.0f,   6.0f,   7.0f,
         8.0f,   9.0f,  10.0f,  11.0f,
        12.0f,  13.0f,  14.0f,  15.0f
    );
    REQUIRE(m.get_column(0) == approx(eu::v4( 0.0f,   1.0f,  2.0f,  3.0f )));
    REQUIRE(m.get_column(1) == approx(eu::v4( 4.0f,   5.0f,  6.0f,  7.0f )));
    REQUIRE(m.get_column(2) == approx(eu::v4( 8.0f,   9.0f, 10.0f, 11.0f)));
    REQUIRE(m.get_column(3) == approx(eu::v4(12.0f,  13.0f, 14.0f, 15.0f)));
}

TEST_CASE("mat4-rowmajor", "[mat]")
{
    const auto m = eu::m4::from_row_major
    (
         0.0f,   1.0f,  2.0f,   3.0f,
         4.0f,   5.0f,  6.0f,   7.0f,
         8.0f,   9.0f, 10.0f,  11.0f,
        12.0f,  13.0f, 14.0f,  15.0f
    );
    REQUIRE(m.get_column(0) == approx(eu::v4(0.0f,  4.0f,   8.0f, 12.0f)));
    REQUIRE(m.get_column(1) == approx(eu::v4(1.0f,  5.0f,   9.0f, 13.0f)));
    REQUIRE(m.get_column(2) == approx(eu::v4(2.0f,  6.0f,  10.0f, 14.0f)));
    REQUIRE(m.get_column(3) == approx(eu::v4(3.0f,  7.0f,  11.0f, 15.0f)));
}

TEST_CASE("mat4-rowmajor_row", "[mat]")
{
    const auto m = eu::m4::from_row_major
    (
         0.0f,   1.0f,   2.0f,   3.0f,
         4.0f,   5.0f,   6.0f,   7.0f,
         8.0f,   9.0f,  10.0f,  11.0f,
        12.0f,  13.0f,  14.0f,  15.0f
    );
    REQUIRE(m.get_row(0) == approx(eu::v4( 0.0f,   1.0f,   2.0f,   3.0f)));
    REQUIRE(m.get_row(1) == approx(eu::v4( 4.0f,   5.0f,   6.0f,   7.0f)));
    REQUIRE(m.get_row(2) == approx(eu::v4( 8.0f,   9.0f,  10.0f,  11.0f)));
    REQUIRE(m.get_row(3) == approx(eu::v4(12.0f,  13.0f,  14.0f,  15.0f)));
}

TEST_CASE("mat4-identity", "[mat]")
{
    const auto m = eu::m4::from_row_major
    (
        1.0f,  0.0f,  0.0f,  0.0f,
        0.0f,  1.0f,  0.0f,  0.0f,
        0.0f,  0.0f,  1.0f,  0.0f,
        0.0f,  0.0f,  0.0f,  1.0f
    );
    const auto i = eu::m4_identity;
    REQUIRE(i.get_column(0) == approx(m.get_column(0)));
    REQUIRE(i.get_column(1) == approx(m.get_column(1)));
    REQUIRE(i.get_column(2) == approx(m.get_column(2)));
    REQUIRE(i.get_column(3) == approx(m.get_column(3)));
}

TEST_CASE("mat4-index", "[mat]")
{
    const auto m = eu::m4::from_row_major
    (
         0.0f,   1.0f,   2.0f,   3.0f,
         4.0f,   5.0f,   6.0f,   7.0f,
         8.0f,   9.0f,  10.0f,  11.0f,
        12.0f,  13.0f,  14.0f,  15.0f
    );
    REQUIRE( 3.0f == approx(m.get(0, 3)));
    REQUIRE(15.0f == approx(m.get(3, 3)));
}

TEST_CASE("mat4-major", "[mat]")
{
    const auto m = eu::m4::from_row_major
        (
             0.0f,   1.0f,   2.0f,   3.0f,
             4.0f,   5.0f,   6.0f,   7.0f,
             8.0f,   9.0f,  10.0f,  11.0f,
            12.0f,  13.0f,  14.0f,  15.0f
        )
        .get_major();
    REQUIRE(eu::v4(0.0f, 5.0f, 10.0f, 15.0f) == approx(m));
}

TEST_CASE("mat4-transposed", "[mat]")
{
    const auto m = eu::m4::from_row_major
        (
             0.0f,   1.0f,   2.0f,   3.0f,
             4.0f,   5.0f,   6.0f,   7.0f,
             8.0f,   9.0f,  10.0f,  11.0f,
            12.0f,  13.0f,  14.0f,  15.0f
        )
        .get_transposed()
        ;
    REQUIRE(m == approx(eu::m4::from_col_major
        (
             0.0f,   1.0f,   2.0f,   3.0f,
             4.0f,   5.0f,   6.0f,   7.0f,
             8.0f,   9.0f,  10.0f,  11.0f,
            12.0f,  13.0f,  14.0f,  15.0f
        )
    ));
}

TEST_CASE("mat4-axis", "[mat]")
{
    const auto m = eu::m4_identity;
    REQUIRE(eu::n3::to_unit({1.0f, 0.0f, 0.0f}) == approx(m.get_x_axis()));
    REQUIRE(eu::n3::to_unit({0.0f, 1.0f, 0.0f}) == approx(m.get_y_axis()));
    REQUIRE(eu::n3::to_unit({0.0f, 0.0f, 1.0f}) == approx(m.get_z_axis()));
}

TEST_CASE("mat4-math", "[mat]")
{
    const auto lhs = eu::m4::from_row_major
    (
         0.0f,  1.0f,  2.0f,  3.0f,
         4.0f,  5.0f,  6.0f,  7.0f,
         8.0f,  9.0f, 10.0f, 11.0f,
        12.0f, 13.0f, 14.0f, 15.0f
    );
    const auto rhs = eu::m4::from_row_major
    (
        16.0f, 17.0f, 18.0f, 19.0f,
        20.0f, 21.0f, 22.0f, 23.0f,
        24.0f, 25.0f, 26.0f, 27.0f,
        28.0f, 29.0f, 30.0f, 31.0f
    );

    SECTION("multiply")
    {
        const auto m = lhs * rhs;
        // thanks wolfram alpha for the result
        // http://www.wolframalpha.com/widgets/view.jsp?id=cc71c2e95a80c217564d530fd8297b0e
        // simplify({{0,1,2,3},{4,5,6,7},{8,9,10,11},{12,13,14,15}}.{{16,17,18,19},{20,21,22,23},{24,25,26,27},{28,29,30,31}})
        REQUIRE
        (
            m == approx(eu::m4::from_row_major
            (
                 152.0f,  158.0f,  164.0f,  170.0f,
                 504.0f,  526.0f,  548.0f,  570.0f,
                 856.0f,  894.0f,  932.0f,  970.0f,
                1208.0f, 1262.0f, 1316.0f, 1370.0f
            ))
        );
    }

    SECTION("add")
    {
        const auto m = lhs + rhs;
        REQUIRE
        (
            m == approx(eu::m4::from_row_major
            (
                16.0f, 18.0f, 20.0f, 22.0f,
                24.0f, 26.0f, 28.0f, 30.0f,
                32.0f, 34.0f, 36.0f, 38.0f,
                40.0f, 42.0f, 44.0f, 46.0f
            ))
        );
    }

    SECTION("sub")
    {
        const auto m = rhs - lhs;
        REQUIRE
        (
            m == approx(eu::m4::from_row_major
            (
                16.0f, 16.0f, 16.0f, 16.0f,
                16.0f, 16.0f, 16.0f, 16.0f,
                16.0f, 16.0f, 16.0f, 16.0f,
                16.0f, 16.0f, 16.0f, 16.0f
            ))
        );
    }
}

TEST_CASE("mat4-inverse", "[mat]")
{
    const auto m0 = eu::m4::from_col_major
    (
        0.6f, 0.2f, 0.3f, 0.4f,
        0.2f, 0.7f, 0.5f, 0.3f,
        0.3f, 0.5f, 0.7f, 0.2f,
        0.4f, 0.3f, 0.2f, 0.6f
    );

    auto inv0 = m0;
    REQUIRE(inv0.invert());

    // todo(Gustav): check matrix values
}


////////////////////////////////////////////////////////////////////////////////



TEST_CASE("mat4-TestTransformation", "[mat]")
{
    const auto src = eu::v3(1.0f, 2.0f, 3.0f);
    const auto translated =
        eu::m4::from_translation(src)
        .get_translation()
        ;
    REQUIRE(translated == approx(src));
}


TEST_CASE("mat4-TestRight", "[mat]")
{
    REQUIRE(eu::m4_identity.get_x_axis() == approx(eu::common::x_axis));
}


TEST_CASE("mat4-TestUp", "[mat]")
{
    REQUIRE(eu::m4_identity.get_y_axis() == approx(eu::common::y_axis));
}



TEST_CASE("mat4-TestIn", "[mat]")
{
    REQUIRE(eu::m4_identity.get_z_axis() == approx(eu::common::z_axis));
}

TEST_CASE("mat4-test", "[mat]")
{
    const eu::m4 start = eu::m4_identity;
    const eu::AxisAngle aa = eu::AxisAngle::from_right_hand_around
    (
        eu::common::up,
        eu::An::from_degrees(-90)
    );
    const eu::v3 to_transform{0.0f, 0.0f, -5.0f};
    const eu::v3 result{5.0f, 0.0f, 0.0f};

    SECTION("TestRotationAxisAngle")
    {
        const auto r = start.get_rotated(aa).get_transform_point(to_transform);
        REQUIRE(r == approx(result));
    }
}


TEST_CASE("mat4-TestCombined_RT", "[mat]")
{
    const auto r = eu::m4_identity
        .get_rotated
        (
            eu::AxisAngle::from_right_hand_around
            (
                eu::common::up,
                eu::An::from_degrees(-90)
            )
        )
        .get_translated(eu::v3{0.0f, 0.0f, -5.0f})
        .get_transform_point(eu::v3{0.0f, 0.0f, 0.0f})
        ;
    REQUIRE(r == approx(eu::v3{5.0f, 0.0f, 0.0f}));
}

TEST_CASE("mat4-TestCombined2_RT", "[mat]")
{
    const auto r =
        eu::m4_identity
        .get_rotated
        (
            eu::AxisAngle::from_right_hand_around
            (
                eu::common::up, eu::An::from_degrees(90)
            )
        )
        .get_translated(eu::v3(0, 0, -5))
        .get_transform_point(eu::v3(0, 0, 0))
        ;
    REQUIRE(r == approx(eu::v3(-5, 0, 0)));
}

TEST_CASE("mat4-TestCombined_TR", "[mat]")
{
    const auto r = eu::m4_identity
        .get_translated(eu::v3(0, 0, 5))
        .get_rotated
        (
            eu::AxisAngle::from_right_hand_around
            (
                eu::common::up,
                eu::An::from_degrees(-90)
            )
        )
        .get_transform_point(eu::v3(0, 0, 0))
        ;
    REQUIRE(r == approx(eu::v3(0, 0, 5)));
}

TEST_CASE("mat4-TestTranslation", "[mat]")
{
    const auto r = eu::m4_identity
        .get_translated(eu::v3(1, 2, 3))
        .get_transform_point(eu::v3(7, 8, 9))
        ;
    REQUIRE(r == approx(eu::v3(8, 10, 12)));
}


TEST_CASE("mat4-TestIentityTransform", "[mat]")
{
    const auto r = eu::m4_identity
        .get_transform_point(eu::v3(1, 2, 3))
        ;
    REQUIRE(r == approx(eu::v3(1, 2, 3)));
}

TEST_CASE("mat4-TestIentityMultiply", "[mat]")
{
    REQUIRE(eu::m4_identity * eu::m4_identity == approx(eu::m4_identity));
}

TEST_CASE("mat4-TestVec4Multiply", "[mat]")
{
    const auto lhs = eu::m4::from_row_major
    (
         0.0f,  1.0f,  2.0f,  3.0f,
         4.0f,  5.0f,  6.0f,  7.0f,
         8.0f,  9.0f, 10.0f, 11.0f,
        12.0f, 13.0f, 14.0f, 15.0f
    );
    const auto m = lhs * eu::v4(16, 17, 18, 19);
    // simplify({{0,1,2,3},{4,5,6,7},{8,9,10,11},{12,13,14,15}}.{16,17,18,19})
    REQUIRE(m == approx(eu::v4(110, 390, 670, 950)));
}

