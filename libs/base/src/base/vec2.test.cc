#include "base/vec2.h"

#include "tests/approx_equal.h"

#include "catch2/catch_all.hpp"

using namespace eu::tests;
using namespace eu;


TEST_CASE("vec2-div_assign", "[vec2]")
{
    auto v = v2(2.0f, 4.0f);
    v /= 2;
    REQUIRE(v == approx(v2{ 1.0f, 2.0f }));
}


TEST_CASE("vec2-length_squared", "[vec2]")
{
    const auto v = v2(1.0f, 2.0f).get_length_squared();
    REQUIRE(v == approx(5.0f));
}


TEST_CASE("vec2-length", "[vec2]")
{
    const auto v = v2(0.0f, 3.0f).get_length();
    REQUIRE(v == approx(3.0f));
}


TEST_CASE("vec2-get_normalized", "[vec2]")
{
    const auto v = v2(0.0f, 3.0f).get_normalized();
    REQUIRE(v == approx(n2{ 0.0f, 1.0f }));
}


TEST_CASE("vec2-dot", "[vec2]")
{
    const auto r = dot(v2(1.0f, 2.0f), v2(3.0f, 4.0f));
    REQUIRE(r == approx(11.0f));
}


TEST_CASE("vec2-constructor-tuple", "[vec2]")
{
    const auto t = std::make_tuple(1.0f, 2.0f);
    const auto v = v2(t);
    REQUIRE(v.x == approx(1.0f));
    REQUIRE(v.y == approx(2.0f));
}

TEST_CASE("vec2-get_data_ptr", "[vec2]")
{
    auto v = v2(1.0f, 2.0f);
    float* data_ptr = v.get_data_ptr();
    REQUIRE(data_ptr[0] == approx(1.0f));
    REQUIRE(data_ptr[1] == approx(2.0f));
    data_ptr[0] = 10.0f;
    data_ptr[1] = 20.0f;
    REQUIRE(v == approx(v2{10.0, 20.0f}));

    const auto cv = v2(3.0f, 4.0f);
    const float* const_data_ptr = cv.get_data_ptr();
    REQUIRE(const_data_ptr[0] == approx(3.0f));
    REQUIRE(const_data_ptr[1] == approx(4.0f));
}

TEST_CASE("vec2-get_rotated", "[vec2]")
{
    const auto v = v2(1.0f, 0.0f).get_rotated(An::from_degrees(90));
    REQUIRE(v == approx(v2{0.0f, 1.0f}));
}

TEST_CASE("vec2-get_flipped_y", "[vec2]")
{
    const auto v = v2(1.0f, 2.0f).get_flipped_y();
    REQUIRE(v == approx(v2{1.0f, -2.0f}));
}

TEST_CASE("vec2-from_to", "[vec2]")
{
    const auto from = v2(1.0f, 2.0f);
    const auto to = v2(4.0f, 6.0f);
    const auto v = v2::from_to(from, to);
    REQUIRE(v == approx(v2{3.0f, 4.0f}));
}

TEST_CASE("vec2-from", "[vec2]")
{
    const auto n = n2(1.0f, 0.0f);
    const auto v = v2::from(n);
    REQUIRE(v == approx(v2{1.0f, 0.0f}));
}

TEST_CASE("n2-get_data_ptr", "[vec2]")
{
    const auto n = n2(1.0f, 0.0f);
    const float* data_ptr = n.get_data_ptr();
    REQUIRE(data_ptr[0] == approx(1.0f));
    REQUIRE(data_ptr[1] == approx(0.0f));
}

TEST_CASE("n2-get_rotated", "[vec2]")
{
    const auto n = n2(1.0f, 0.0f).get_rotated(An::from_degrees(90));
    REQUIRE(n == approx(n2{0.0f, 1.0f}));
}

TEST_CASE("n2-get_flipped_y", "[vec2]")
{
    const auto n = n2(0.0f, 1.0f).get_flipped_y();
    REQUIRE(n == approx(n2{0.0f, -1.0f}));
}

TEST_CASE("lerp_vec2f", "[vec2]")
{
    const auto from = v2(1.0f, 2.0f);
    const auto to = v2(4.0f, 6.0f);
    const auto v = lerp_vec2f(from, 0.5f, to);
    REQUIRE(v == approx(v2{2.5f, 4.0f}));
}

TEST_CASE("vec2-print", "[vec2]")
{
    const auto v = v2(1.0f, 2.0f);

    SECTION("fmt")
    {
        std::string str = fmt::format("{0}", v);
        CHECK(str == "(1, 2)");
    }

    SECTION("stream")
    {
        std::ostringstream oss;
        oss << v;
        CHECK(oss.str() == "(1, 2)");
    }
}

TEST_CASE("n2-print", "[vec2]")
{
    const auto n = n2(1.0f, 0.0f);

    SECTION("fmt")
    {
        std::string str = fmt::format("{0}", n);
        CHECK(str == "(1, 0)");
    }

    SECTION("stream")
    {
        std::ostringstream oss;
        oss << n;
        CHECK(oss.str() == "(1, 0)");
    }
}

TEST_CASE("vec2-operators", "[vec2]")
{
    const auto lhs = v2(1.0f, 2.0f);
    const auto rhs = v2(3.0f, 4.0f);

    SECTION("addition")
    {
        const auto result = lhs + rhs;
        REQUIRE(result == approx(v2{4.0f, 6.0f}));
    }

    SECTION("subtraction")
    {
        const auto result = lhs - rhs;
        REQUIRE(result == approx(v2{-2.0f, -2.0f}));
    }

    SECTION("multiplication with scalar")
    {
        const auto result = lhs * 2.0f;
        REQUIRE(result == approx(v2{2.0f, 4.0f}));
    }

    SECTION("multiplication with scalar (reversed)")
    {
        const auto result = 2.0f * rhs;
        REQUIRE(result == approx(v2{6.0f, 8.0f}));
    }

    SECTION("division with scalar")
    {
        const auto result = lhs / 2.0f;
        REQUIRE(result == approx(v2{0.5f, 1.0f}));
    }

    SECTION("negation")
    {
        const auto result = -lhs;
        REQUIRE(result == approx(v2{-1.0f, -2.0f}));
    }
}

TEST_CASE("n2-operators", "[vec2]")
{
    const auto n1 = n2(1.0f, 0.0f);

    SECTION("multiplication with scalar")
    {
        const auto result = n1 * 2.0f;
        REQUIRE(result == approx(v2{2.0f, 0.0f}));
    }

    SECTION("multiplication with scalar (reversed)")
    {
        const auto result = 2.0f * n1;
        REQUIRE(result == approx(v2{2.0f, 0.0f}));
    }

    SECTION("negation")
    {
        const auto result = -n1;
        REQUIRE(result == approx(n2{-1.0f, 0.0f}));
    }
}
