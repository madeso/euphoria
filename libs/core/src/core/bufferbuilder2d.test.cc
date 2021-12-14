#include "core/bufferbuilder2d.h"

#include "core/str.h"

#include "tests/vectorequals.h"
#include "tests/approx.h"

#include "catch.hpp"


using namespace euphoria::core;
using euphoria::tests::FalseString;


namespace
{
    FalseString
    vector_is_same(const std::vector<unsigned int> lhs, const std::vector<unsigned int>& rhs)
    {
        return euphoria::tests::vector_is_equal
        (
            lhs,
            rhs,
            [](unsigned int f) -> std::string
            { return StringBuilder() << f; },
            [](unsigned int a, unsigned int b) -> FalseString
            {
                if (a == b)
                {
                    return FalseString::create_true();
                } else
                {
                    return FalseString::create_false
                    (
                        StringBuilder() << a << " != " << b
                    );
                }
            }
        );
    }


    FalseString
    vector_is_same(const std::vector<float> lhs, const std::vector<float>& rhs)
    {
        return euphoria::tests::vector_is_equal
        (
            lhs,
            rhs,
            [](float f) -> std::string
            { return StringBuilder() << f; },
            [](float a, float b) -> FalseString
            {
                if (euphoria::tests::approx(a) == b)
                {
                    return FalseString::create_true();
                } else
                {
                    return FalseString::create_false
                    (
                        StringBuilder() << a << " != " << b
                    );
                }
            }
        );
    }
}


TEST_CASE("bufferbuilder2d-default-is-empty", "[bufferbuilder2d]")
{
    auto bb = BufferBuilder2{};
    CHECK(bb.data.empty());
    CHECK(bb.tris.empty());
}

TEST_CASE("bufferbuilder2d-add-points", "[bufferbuilder2d]")
{
    auto bb = BufferBuilder2{};
    bb.add_vertex({0.0f, 1.0f, 2.0f, 3.0f});
    bb.add_vertex({55.0f, 20.0f, 0.0f, -2.0f});
    CHECK(vector_is_same(bb.data, {0.0f, 1.0f, 2.0f, 3.0f, 55.0f, 20.0f, 0.0f, -2.0f}));
    CHECK(bb.tris.empty());
}

TEST_CASE("bufferbuilder2d-triangles", "[bufferbuilder2d]")
{
    auto bb = BufferBuilder2{};
    bb.add_vertex({0.0f, 0.0f, 0.0f, 0.0f});
    bb.add_vertex({0.0f, 1.0f, 0.0f, 0.0f});
    bb.add_vertex({1.0f, 0.0f, 0.0f, 0.0f});
    CHECK(bb.data.size() == 12);
    SECTION("ok - ccw")
    {
        bb.add_triangle(0, 1, 2);
        CHECK(vector_is_same(bb.tris, {0, 1, 2}));
    }
    SECTION("bad - cw")
    {
        CHECK_THROWS(bb.add_triangle(0, 2, 1));
    }
}

TEST_CASE("bufferbuilder2d-quad", "[bufferbuilder2d]")
{
    auto bb = BufferBuilder2{};
    const auto z = BufferPoint2{0.0f, 0.0f, 0.0f, 0.0f};
    const auto x = BufferPoint2{1.0f, 0.0f, 0.0f, 0.0f};
    const auto y = BufferPoint2{0.0f, 1.0f, 0.0f, 0.0f};
    const auto xy = BufferPoint2{1.0f, 1.0f, 0.0f, 0.0f};
    SECTION("bad - cw")
    {
        CHECK_THROWS(bb.add_quad(z, y, xy, x));
    }
    SECTION("bad - ccw")
    {
        CHECK_THROWS(bb.add_quad(z, x, xy, y));
    }
    SECTION("ok bottom-top, left-right")
    {
        bb.add_quad(z, x, y, xy);
        // todo(Gustav): add checks
    }
    SECTION("bad - top->bottom, left->right")
    {
        CHECK_THROWS(bb.add_quad(y, xy, z, x));
    }
}

