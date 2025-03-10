#include "base/mat4.h"
#include "base/numeric.h"

#include "tests/approx_equal.h"

#include "catch2/catch_all.hpp"


using namespace eu;
using namespace eu::tests;

TEST_CASE("mat4-colmajor", "[mat]")
{
    const auto m = m4::from_col_major
    (
         0.0f,   1.0f,   2.0f,   3.0f,
         4.0f,   5.0f,   6.0f,   7.0f,
         8.0f,   9.0f,  10.0f,  11.0f,
        12.0f,  13.0f,  14.0f,  15.0f
    );
    REQUIRE(m.get_column(0) == approx(v4( 0.0f,   1.0f,  2.0f,  3.0f )));
    REQUIRE(m.get_column(1) == approx(v4( 4.0f,   5.0f,  6.0f,  7.0f )));
    REQUIRE(m.get_column(2) == approx(v4( 8.0f,   9.0f, 10.0f, 11.0f)));
    REQUIRE(m.get_column(3) == approx(v4(12.0f,  13.0f, 14.0f, 15.0f)));
}

TEST_CASE("mat4-rowmajor", "[mat]")
{
    const auto m = m4::from_row_major
    (
         0.0f,   1.0f,  2.0f,   3.0f,
         4.0f,   5.0f,  6.0f,   7.0f,
         8.0f,   9.0f, 10.0f,  11.0f,
        12.0f,  13.0f, 14.0f,  15.0f
    );
    REQUIRE(m.get_column(0) == approx(v4(0.0f,  4.0f,   8.0f, 12.0f)));
    REQUIRE(m.get_column(1) == approx(v4(1.0f,  5.0f,   9.0f, 13.0f)));
    REQUIRE(m.get_column(2) == approx(v4(2.0f,  6.0f,  10.0f, 14.0f)));
    REQUIRE(m.get_column(3) == approx(v4(3.0f,  7.0f,  11.0f, 15.0f)));
}

TEST_CASE("mat4-rowmajor_row", "[mat]")
{
    const auto m = m4::from_row_major
    (
         0.0f,   1.0f,   2.0f,   3.0f,
         4.0f,   5.0f,   6.0f,   7.0f,
         8.0f,   9.0f,  10.0f,  11.0f,
        12.0f,  13.0f,  14.0f,  15.0f
    );
    REQUIRE(m.get_row(0) == approx(v4( 0.0f,   1.0f,   2.0f,   3.0f)));
    REQUIRE(m.get_row(1) == approx(v4( 4.0f,   5.0f,   6.0f,   7.0f)));
    REQUIRE(m.get_row(2) == approx(v4( 8.0f,   9.0f,  10.0f,  11.0f)));
    REQUIRE(m.get_row(3) == approx(v4(12.0f,  13.0f,  14.0f,  15.0f)));
}

TEST_CASE("mat4-identity", "[mat]")
{
    const auto m = m4::from_row_major
    (
        1.0f,  0.0f,  0.0f,  0.0f,
        0.0f,  1.0f,  0.0f,  0.0f,
        0.0f,  0.0f,  1.0f,  0.0f,
        0.0f,  0.0f,  0.0f,  1.0f
    );
    const auto i = m4_identity;
    REQUIRE(i.get_column(0) == approx(m.get_column(0)));
    REQUIRE(i.get_column(1) == approx(m.get_column(1)));
    REQUIRE(i.get_column(2) == approx(m.get_column(2)));
    REQUIRE(i.get_column(3) == approx(m.get_column(3)));
}

TEST_CASE("mat4-index", "[mat]")
{
    const auto m = m4::from_row_major
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
    const auto m = m4::from_row_major
        (
             0.0f,   1.0f,   2.0f,   3.0f,
             4.0f,   5.0f,   6.0f,   7.0f,
             8.0f,   9.0f,  10.0f,  11.0f,
            12.0f,  13.0f,  14.0f,  15.0f
        )
        .get_major();
    REQUIRE(v4(0.0f, 5.0f, 10.0f, 15.0f) == approx(m));
}

TEST_CASE("mat4-transposed", "[mat]")
{
    const auto m = m4::from_row_major
        (
             0.0f,   1.0f,   2.0f,   3.0f,
             4.0f,   5.0f,   6.0f,   7.0f,
             8.0f,   9.0f,  10.0f,  11.0f,
            12.0f,  13.0f,  14.0f,  15.0f
        )
        .get_transposed()
        ;
    REQUIRE(m == approx(m4::from_col_major
        (
             0.0f,   1.0f,   2.0f,   3.0f,
             4.0f,   5.0f,   6.0f,   7.0f,
             8.0f,   9.0f,  10.0f,  11.0f,
            12.0f,  13.0f,  14.0f,  15.0f
        )
    ));
}

TEST_CASE("mat4-get_transposed", "[mat]")
{
    const auto m = m4::from_row_major
    (
         0.0f,   1.0f,   2.0f,   3.0f,
         4.0f,   5.0f,   6.0f,   7.0f,
         8.0f,   9.0f,  10.0f,  11.0f,
        12.0f,  13.0f,  14.0f,  15.0f
    );
    const auto transposed = m.get_transposed();
    REQUIRE(transposed == approx(m4::from_col_major
    (
         0.0f,   1.0f,   2.0f,   3.0f,
         4.0f,   5.0f,   6.0f,   7.0f,
         8.0f,   9.0f,  10.0f,  11.0f,
        12.0f,  13.0f,  14.0f,  15.0f
    )));
}

TEST_CASE("mat4-axis", "[mat]")
{
    const auto m = m4_identity;
    REQUIRE(n3{1.0f, 0.0f, 0.0f} == approx(m.get_x_axis()));
    REQUIRE(n3{0.0f, 1.0f, 0.0f} == approx(m.get_y_axis()));
    REQUIRE(n3{0.0f, 0.0f, 1.0f} == approx(m.get_z_axis()));
}

TEST_CASE("mat4-math", "[mat]")
{
    const auto lhs = m4::from_row_major
    (
         0.0f,  1.0f,  2.0f,  3.0f,
         4.0f,  5.0f,  6.0f,  7.0f,
         8.0f,  9.0f, 10.0f, 11.0f,
        12.0f, 13.0f, 14.0f, 15.0f
    );
    const auto rhs = m4::from_row_major
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
            m == approx(m4::from_row_major
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
            m == approx(m4::from_row_major
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
            m == approx(m4::from_row_major
            (
                16.0f, 16.0f, 16.0f, 16.0f,
                16.0f, 16.0f, 16.0f, 16.0f,
                16.0f, 16.0f, 16.0f, 16.0f,
                16.0f, 16.0f, 16.0f, 16.0f
            ))
        );
    }
}

TEST_CASE("mat4-get_inverted", "[mat]")
{
    const auto m = m4::from_row_major
    (
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    const auto inv = m.get_inverted();
    REQUIRE(inv == approx(m4_identity));
}

TEST_CASE("mat4-inverse", "[mat]")
{
    // example from https://www.youtube.com/watch?v=95dYWsZEXmM
    const auto m0 = m4::from_col_major
    (
        1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 3.0f, 1.0f, 2.0f,
        2.0f, 3.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 2.0f, 1.0f
    );

    auto inv = m0;
    REQUIRE(inv.invert());

    const auto res = m4::from_col_major
    (
        -3.0f, -0.5f,   1.5f,   1.0f,
         1.0f,  0.25f, -0.25f, -0.5f,
         3.0f,  0.25f, -1.25f, -0.5f,
        -3.0f,  0.0f,   1.0f,   1.0f
    );
    CHECK(inv == approx(res));
}

TEST_CASE("mat4-from_major", "[mat]")
{
    const auto major = v4(1.0f, 2.0f, 3.0f, 4.0f);
    const auto m = m4::from_major(major);
    REQUIRE(m.get_major() == approx(major));
}


TEST_CASE("mat4-get_transformed_vec_point", "[mat]")
{
    const auto m = m4::from_translation(v3(1.0f, 2.0f, 3.0f));
    const auto v = v3(4.0f, 5.0f, 6.0f);
    const auto transformed = m.get_transformed_point(v);
    REQUIRE(transformed == approx(v3(5.0f, 7.0f, 9.0f)));
}

TEST_CASE("mat4-get_transformed_vec_normal", "[mat]")
{
    const auto m = m4::from_rot_x(An::from_degrees(90));
    const auto v = n3(0.0f, 1.0f, 0.0f);
    const auto transformed = m.get_transformed_vec(v);
    REQUIRE(transformed == approx(n3(0.0f, 0.0f, 1.0f)));
}

TEST_CASE("mat4-from_quaternion", "[mat]")
{
    const auto q = Q::from(AA{n3(0.0f, 1.0f, 0.0f), An::from_degrees(90)});
    const auto m = m4::from(q);
    REQUIRE(m.has_value());
    const auto v = v3(1.0f, 0.0f, 0.0f);
    const auto transformed = m->get_transformed_point(v);
    REQUIRE(transformed == approx(v3(0.0f, 0.0f, -1.0f)));
}

////////////////////////////////////////////////////////////////////////////////



TEST_CASE("mat4-TestTransformation", "[mat]")
{
    const auto src = v3(1.0f, 2.0f, 3.0f);
    const auto translated =
        m4::from_translation(src)
        .get_translation()
        ;
    REQUIRE(translated == approx(src));
}


TEST_CASE("mat4-TestRight", "[mat]")
{
    REQUIRE(m4_identity.get_x_axis() == approx(kk::x_axis));
}


TEST_CASE("mat4-TestUp", "[mat]")
{
    REQUIRE(m4_identity.get_y_axis() == approx(kk::y_axis));
}



TEST_CASE("mat4-TestIn", "[mat]")
{
    REQUIRE(m4_identity.get_z_axis() == approx(kk::z_axis));
}

TEST_CASE("mat4-test", "[mat]")
{
    const m4 start = m4_identity;
    const AA aa = rha
    (
        kk::up,
        An::from_degrees(-90)
    );
    const v3 to_transform{0.0f, 0.0f, -5.0f};
    const v3 result{5.0f, 0.0f, 0.0f};

    SECTION("TestRotationAxisAngle")
    {
        const auto r = start.get_rotated(aa).get_transformed_point(to_transform);
        REQUIRE(r == approx(result));
    }
}

TEST_CASE("mat4-from-axisangle", "[mat]")
{
    const auto axis = n3{0.0f, 1.0f, 0.0f};
    const auto angle = An::from_degrees(90);
    const auto aa = AA{axis, angle};
    const auto m = m4::from(aa);
    const auto v = v3(1.0f, 0.0f, 0.0f);
    const auto rotated = m.get_transformed_point(v);
    REQUIRE(rotated == approx(v3(0.0f, 0.0f, -1.0f)));
}

TEST_CASE("mat4-TestCombined_RT", "[mat]")
{
    const auto r = m4_identity
        .get_rotated
        (
            rha
            (
                kk::up,
                An::from_degrees(-90)
            )
        )
        .get_translated(v3{0.0f, 0.0f, -5.0f})
        .get_transformed_point(v3{0.0f, 0.0f, 0.0f})
        ;
    REQUIRE(r == approx(v3{5.0f, 0.0f, 0.0f}));
}

TEST_CASE("mat4-TestCombined2_RT", "[mat]")
{
    const auto r =
        m4_identity
        .get_rotated
        (
            rha
            (
                kk::up, An::from_degrees(90)
            )
        )
        .get_translated(v3(0, 0, -5))
        .get_transformed_point(v3(0, 0, 0))
        ;
    REQUIRE(r == approx(v3(-5, 0, 0)));
}

TEST_CASE("mat4-TestCombined_TR", "[mat]")
{
    const auto r = m4_identity
        .get_translated(v3(0, 0, 5))
        .get_rotated
        (
            rha
            (
                kk::up,
                An::from_degrees(-90)
            )
        )
        .get_transformed_point(v3(0, 0, 0))
        ;
    REQUIRE(r == approx(v3(0, 0, 5)));
}

TEST_CASE("mat4-TestTranslation", "[mat]")
{
    const auto r = m4_identity
        .get_translated(v3(1, 2, 3))
        .get_transformed_point(v3(7, 8, 9))
        ;
    REQUIRE(r == approx(v3(8, 10, 12)));
}


TEST_CASE("mat4-TestIentityTransform", "[mat]")
{
    const auto r = m4_identity
        .get_transformed_point(v3(1, 2, 3))
        ;
    REQUIRE(r == approx(v3(1, 2, 3)));
}

TEST_CASE("mat4-TestIentityMultiply", "[mat]")
{
    REQUIRE(m4_identity * m4_identity == approx(m4_identity));
}

TEST_CASE("mat4-TestVec4Multiply", "[mat]")
{
    const auto lhs = m4::from_row_major
    (
         0.0f,  1.0f,  2.0f,  3.0f,
         4.0f,  5.0f,  6.0f,  7.0f,
         8.0f,  9.0f, 10.0f, 11.0f,
        12.0f, 13.0f, 14.0f, 15.0f
    );
    const auto m = lhs * v4(16, 17, 18, 19);
    // simplify({{0,1,2,3},{4,5,6,7},{8,9,10,11},{12,13,14,15}}.{16,17,18,19})
    REQUIRE(m == approx(v4(110, 390, 670, 950)));
}

TEST_CASE("mat4-projection-tests", "[mat]")
{
    // regarding the negative z, in lefthanded it would be positive, but this is righthanded!!

    SECTION("ortho")
    {
        const auto o = m4::create_ortho_lrud(0.0f, 10.0f, 10.0f, 0.0f, 0.0f, 10.0f);
        
        CHECK(o.get_transformed_point({0.0, 0.0, 0.0}) == approx(v3(-1.0f, -1.0f, -1.0f)));
        CHECK(o.get_transformed_point({10.0, 10.0, -10.0}) == approx(v3(1.0f, 1.0f, 1.0f)));
    }

    SECTION("perspective")
    {
        const auto o = m4::create_perspective(An::from_degrees(90), 1.0f, 1.0f, 10.0f);
        CHECK(o.get_transformed_point({0.0, 0.0, -1.0}) == approx(v3(0.0f, 0.0f, -1.0f)));
        CHECK(o.get_transformed_point({10.0, 10.0, -10.0}) == approx(v3(1.0f, 1.0f, 1.0f)));
    }
}

TEST_CASE("mat4-rotation-x", "[mat]")
{
    const auto angle = An::from_degrees(90);
    const auto m = m4::from_rot_x(angle);
    const auto v = v3(0.0f, 1.0f, 0.0f);
    const auto rotated = m.get_transformed_point(v);
    REQUIRE(rotated == approx(v3(0.0f, 0.0f, 1.0f)));
}

TEST_CASE("mat4-rotation-y", "[mat]")
{
    const auto angle = An::from_degrees(90);
    const auto m = m4::from_rot_y(angle);
    const auto v = v3(1.0f, 0.0f, 0.0f);
    const auto rotated = m.get_transformed_point(v);
    REQUIRE(rotated == approx(v3(0.0f, 0.0f, -1.0f)));
}

TEST_CASE("mat4-rotation-z", "[mat]")
{
    const auto angle = An::from_degrees(90);
    const auto m = m4::from_rot_z(angle);
    const auto v = v3(1.0f, 0.0f, 0.0f);
    const auto rotated = m.get_transformed_point(v);
    REQUIRE(rotated == approx(v3(0.0f, 1.0f, 0.0f)));
}

TEST_CASE("mat4-column_major_data_ptr", "[mat]")
{
    const auto m = m4::from_row_major
    (
         0.0f,   1.0f,   2.0f,   3.0f,
         4.0f,   5.0f,   6.0f,   7.0f,
         8.0f,   9.0f,  10.0f,  11.0f,
        12.0f,  13.0f,  14.0f,  15.0f
    );
    const float* data_ptr = m.get_column_major_data_ptr();
    REQUIRE(data_ptr[0] == 0.0f);
    REQUIRE(data_ptr[1] == 4.0f);
    REQUIRE(data_ptr[2] == 8.0f);
    REQUIRE(data_ptr[3] == 12.0f);
    REQUIRE(data_ptr[4] == 1.0f);
    REQUIRE(data_ptr[5] == 5.0f);
    REQUIRE(data_ptr[6] == 9.0f);
    REQUIRE(data_ptr[7] == 13.0f);
    REQUIRE(data_ptr[8] == 2.0f);
    REQUIRE(data_ptr[9] == 6.0f);
    REQUIRE(data_ptr[10] == 10.0f);
    REQUIRE(data_ptr[11] == 14.0f);
    REQUIRE(data_ptr[12] == 3.0f);
    REQUIRE(data_ptr[13] == 7.0f);
    REQUIRE(data_ptr[14] == 11.0f);
    REQUIRE(data_ptr[15] == 15.0f);
}

TEST_CASE("mat4-modify_column_major_data_ptr", "[mat]")
{
    auto m = m4::from_row_major
    (
         0.0f,   1.0f,   2.0f,   3.0f,
         4.0f,   5.0f,   6.0f,   7.0f,
         8.0f,   9.0f,  10.0f,  11.0f,
        12.0f,  13.0f,  14.0f,  15.0f
    );
    float* data_ptr = m.get_column_major_data_ptr();
    data_ptr[0] = 16.0f;
    data_ptr[5] = 17.0f;
    data_ptr[10] = 18.0f;
    data_ptr[15] = 19.0f;

    REQUIRE(m.get(0, 0) == 16.0f);
    REQUIRE(m.get(1, 1) == 17.0f);
    REQUIRE(m.get(2, 2) == 18.0f);
    REQUIRE(m.get(3, 3) == 19.0f);
}

TEST_CASE("mat4-print", "[mat]")
{
    const auto m = m4::from_row_major
    (
         0.0f,   1.0f,   2.0f,   3.0f,
         4.0f,   5.0f,   6.0f,   7.0f,
         8.0f,   9.0f,  10.0f,  11.0f,
        12.0f,  13.0f,  14.0f,  15.0f
    );

    SECTION("fmt")
    {
        std::string str = fmt::format("{0}", m);
        CHECK(str == "0 1 2 3\n4 5 6 7\n8 9 10 11\n12 13 14 15");
    }

    SECTION("stream")
    {
        std::ostringstream oss;
        oss << m;
        CHECK(oss.str() == "0 1 2 3\n4 5 6 7\n8 9 10 11\n12 13 14 15");
    }
}
