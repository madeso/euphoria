#include "base/vec4.h"

#include "tests/approx_equal.h"

#include "catch2/catch_all.hpp"

using namespace eu::tests;
using namespace eu;

TEST_CASE("vec4-constructor_unique", "[vec4]")
{
    const auto v = v4(1.0f, 2.0f, 3.0f, 4.0f);
    CHECK(v.x == approx(1.0f));
    CHECK(v.y == approx(2.0f));
    CHECK(v.z == approx(3.0f));
    CHECK(v.w == approx(4.0f));
}

TEST_CASE("vec4-constructor_vec3", "[vec4]")
{
    const auto v = v4(v3(1.0f, 2.0f, 3.0f), 1.0f);
    CHECK(v.x == approx(1.0f));
    CHECK(v.y == approx(2.0f));
    CHECK(v.z == approx(3.0f));
    CHECK(v.w == approx(1.0f));
}

TEST_CASE("vec4-constructor_array", "[vec4]")
{
    float arr[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    const auto v = v4(arr);
    CHECK(v.x == approx(1.0f));
    CHECK(v.y == approx(2.0f));
    CHECK(v.z == approx(3.0f));
    CHECK(v.w == approx(4.0f));
}

TEST_CASE("vec4-cast_vec3", "[vec4]")
{
    const auto v = v4(1.0f, 2.0f, 3.0f, 0.0f).to_vec3(0.0f);
    CHECK(v.x == approx(1.0f));
    CHECK(v.y == approx(2.0f));
    CHECK(v.z == approx(3.0f));
}

TEST_CASE("v4-print", "[vec4]")
{
    const auto v = v4(1.0f, 2.0f, 3.0f, 4.0f);

    SECTION("fmt")
    {
        std::string str = fmt::format("{0}", v);
        CHECK(str == "(1, 2, 3, 4)");
    }

    SECTION("stream")
    {
        std::ostringstream oss;
        oss << v;
        CHECK(oss.str() == "(1, 2, 3, 4)");
    }
}

TEST_CASE("vec4-get_data_ptr", "[vec4]")
{
    auto v = v4(1.0f, 2.0f, 3.0f, 4.0f);
    float* data_ptr = v.get_data_ptr();
    CHECK(data_ptr[0] == approx(1.0f));
    CHECK(data_ptr[1] == approx(2.0f));
    CHECK(data_ptr[2] == approx(3.0f));
    CHECK(data_ptr[3] == approx(4.0f));
    data_ptr[0] = 10.0f;
    data_ptr[1] = 20.0f;
    data_ptr[2] = 30.0f;
    data_ptr[3] = 40.0f;
    CHECK(v == approx(v4{10.0f, 20.0f, 30.0f, 40.0f}));

    const auto cv = v4(4.0f, 5.0f, 6.0f, 7.0f);
    const float* const_data_ptr = cv.get_data_ptr();
    CHECK(const_data_ptr[0] == approx(4.0f));
    CHECK(const_data_ptr[1] == approx(5.0f));
    CHECK(const_data_ptr[2] == approx(6.0f));
    CHECK(const_data_ptr[3] == approx(7.0f));
}
