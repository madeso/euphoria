#include "base/vec3.h"

#include "tests/approx_equal.h"

#include "catch2/catch_all.hpp"

using namespace eu::tests;

TEST_CASE("vec3-to-unit-test", "[vec3]")
{
    REQUIRE(eu::n3{1.0f, 0.0f, 0.0f} == approx(eu::kk::x_axis));
    REQUIRE(eu::n3{0.0f, 1.0f, 0.0f} == approx(eu::kk::y_axis));
    REQUIRE(eu::n3{0.0f, 0.0f, 1.0f} == approx(eu::kk::z_axis));
}

TEST_CASE("vec3-axis_test", "[vec3]")
{
    REQUIRE(eu::kk::x_axis == approx(eu::v3(1.0f, 0.0f, 0.0f)));
    REQUIRE(eu::kk::y_axis == approx(eu::v3(0.0f, 1.0f, 0.0f)));
    REQUIRE(eu::kk::z_axis == approx(eu::v3(0.0f, 0.0f, 1.0f)));
}

TEST_CASE("vec3-constructor_same", "[vec3]")
{
    const auto v = eu::v3(42.0f);
    REQUIRE(v.x == approx(42.0f));
    REQUIRE(v.y == approx(42.0f));
    REQUIRE(v.z == approx(42.0f));
}

TEST_CASE("vec3-constructor_unique", "[vec3]")
{
    const auto v = eu::v3(1.0f, 2.0f, 3.0f);
    REQUIRE(v.x == approx(1.0f));
    REQUIRE(v.y == approx(2.0f));
    REQUIRE(v.z == approx(3.0f));
}

TEST_CASE("vec3-constructor_tuple", "[vec3]")
{
    const auto t = std::make_tuple(1.0f, 2.0f, 3.0f);
    const auto v = eu::v3(t);
    REQUIRE(v.x == Catch::Approx(1.0f));
    REQUIRE(v.y == Catch::Approx(2.0f));
    REQUIRE(v.z == Catch::Approx(3.0f));
}

TEST_CASE("vec3-constructor_float_array", "[vec3]")
{
    const float arr[3] = {4.0f, 5.0f, 6.0f};
    const auto v = eu::v3(arr);
    REQUIRE(v.x == Catch::Approx(4.0f));
    REQUIRE(v.y == Catch::Approx(5.0f));
    REQUIRE(v.z == Catch::Approx(6.0f));
}

TEST_CASE("vec3-equal", "[vec3]")
{
    REQUIRE(eu::v3(1.0f, 2.0f, 3.0f) == approx(eu::v3(1.0f, 2.0f, 3.0f)));
    REQUIRE_FALSE(eu::v3(1.0f, 2.0f, 3.0f) == approx(eu::v3(3.0f, 2.0f, 1.0f)));
}

TEST_CASE("vec3-neg_test", "[vec3]")
{
    REQUIRE(-eu::v3(1.0f, 2.0f, 3.0f) == approx(eu::v3(-1.0f, -2.0f, -3.0f)));
    REQUIRE(-eu::v3(-1.0f, 2.0f, 3.0f) == approx(eu::v3(1.0f, -2.0f, -3.0f)));
    REQUIRE(-eu::v3(-1.0f, -2.0f, -3.0f) == approx(eu::v3(1.0f, 2.0f, 3.0f)));
}

TEST_CASE("vec3-from_to", "[vec3]")
{
    REQUIRE(eu::v3::from_to(eu::v3(0.0f, 0.0f, 0.0f), eu::v3(0.0f, 0.0f, 0.0f)) == approx(eu::v3(0.0f, 0.0f, 0.0f)));
    REQUIRE(eu::v3::from_to(eu::v3(0.0f, 0.0f, 0.0f), eu::v3(1.0f, 0.0f, 0.0f)) == approx(eu::v3(1.0f, 0.0f, 0.0f)));
    REQUIRE(eu::v3::from_to(eu::v3(0.0f, -5.0f, 0.0f), eu::v3(0.0f, 25.0f, 0.0f)) == approx(eu::v3(0.0f, 30.0f, 0.0f)));
}

TEST_CASE("vec3-get_data_ptr", "[vec3]")
{
    auto v = eu::v3(1.0f, 2.0f, 3.0f);
    float* data_ptr = v.get_data_ptr();
    REQUIRE(data_ptr[0] == Catch::Approx(1.0f));
    REQUIRE(data_ptr[1] == Catch::Approx(2.0f));
    REQUIRE(data_ptr[2] == Catch::Approx(3.0f));
    data_ptr[0] = 10.0f;
    data_ptr[1] = 20.0f;
    data_ptr[2] = 30.0f;
    REQUIRE(v == approx(eu::v3{10.0f, 20.0f, 30.0f}));

    const auto cv = eu::v3(4.0f, 5.0f, 6.0f);
    const float* const_data_ptr = cv.get_data_ptr();
    REQUIRE(const_data_ptr[0] == Catch::Approx(4.0f));
    REQUIRE(const_data_ptr[1] == Catch::Approx(5.0f));
    REQUIRE(const_data_ptr[2] == Catch::Approx(6.0f));
}

TEST_CASE("vec3-from_localspace_rui", "[vec3]")
{
    const auto q = eu::q_identity;
    const auto v = eu::v3::from_localspace_rui(q, 1.0f, 2.0f, 3.0f);
    REQUIRE(v == approx(eu::v3{1.0f, 2.0f, -3.0f}));
}

TEST_CASE("vec3-get_length", "[vec3]")
{
    const auto v = eu::v3(1.0f, 2.0f, 2.0f).get_length();
    REQUIRE(v == Catch::Approx(3.0f));
}

TEST_CASE("vec3-lerp_v3", "[vec3]")
{
    const auto from = eu::v3(1.0f, 2.0f, 3.0f);
    const auto to = eu::v3(4.0f, 5.0f, 6.0f);
    const auto v = eu::lerp_v3(from, 0.5f, to);
    REQUIRE(v == approx(eu::v3{2.5f, 3.5f, 4.5f}));
}

TEST_CASE("v3-print", "[vec3]")
{
    const auto v = eu::v3(1.0f, 2.0f, 3.0f);

    SECTION("fmt")
    {
        std::string str = fmt::format("{0}", v);
        CHECK(str == "(1, 2, 3)");
    }

    SECTION("stream")
    {
        std::ostringstream oss;
        oss << v;
        CHECK(oss.str() == "(1, 2, 3)");
    }
}

TEST_CASE("n3-print", "[vec3]")
{
    const auto n = eu::n3(1.0f, 0.0f, 0.0f);

    SECTION("fmt")
    {
        std::string str = fmt::format("{0}", n);
        CHECK(str == "(1, 0, 0)");
    }

    SECTION("stream")
    {
        std::ostringstream oss;
        oss << n;
        CHECK(oss.str() == "(1, 0, 0)");
    }
}

TEST_CASE("vec3-operators", "[vec3]")
{
    const auto v1 = eu::v3(1.0f, 2.0f, 3.0f);
    const auto v2 = eu::v3(4.0f, 5.0f, 6.0f);

    SECTION("addition")
    {
        const auto result = v1 + v2;
        REQUIRE(result == approx(eu::v3{5.0f, 7.0f, 9.0f}));
    }

    SECTION("subtraction")
    {
        const auto result = v1 - v2;
        REQUIRE(result == approx(eu::v3{-3.0f, -3.0f, -3.0f}));
    }

    SECTION("multiplication with scalar")
    {
        const auto result = v1 * 2.0f;
        REQUIRE(result == approx(eu::v3{2.0f, 4.0f, 6.0f}));
    }

    SECTION("multiplication with scalar (reversed)")
    {
        const auto result = 2.0f * v1;
        REQUIRE(result == approx(eu::v3{2.0f, 4.0f, 6.0f}));
    }

    SECTION("division with scalar")
    {
        const auto result = v1 / 2.0f;
        REQUIRE(result == approx(eu::v3{0.5f, 1.0f, 1.5f}));
    }

    SECTION("negation")
    {
        const auto result = -v1;
        REQUIRE(result == approx(eu::v3{-1.0f, -2.0f, -3.0f}));
    }
}

TEST_CASE("n3-operators", "[vec3]")
{
    const auto n1 = eu::n3(1.0f, 0.0f, 0.0f);
    const auto n2 = eu::n3(0.0f, 1.0f, 0.0f);

    SECTION("multiplication with scalar")
    {
        const auto result = n1 * 2.0f;
        REQUIRE(result == approx(eu::v3{2.0f, 0.0f, 0.0f}));
    }

    SECTION("multiplication with scalar (reversed)")
    {
        const auto result = 2.0f * n1;
        REQUIRE(result == approx(eu::v3{2.0f, 0.0f, 0.0f}));
    }

    SECTION("negation")
    {
        const auto result = -n1;
        REQUIRE(result == approx(eu::n3{-1.0f, 0.0f, 0.0f}));
    }
}
