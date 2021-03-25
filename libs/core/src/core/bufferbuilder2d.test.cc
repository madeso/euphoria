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
    VectorEquals(const std::vector<unsigned int> lhs, const std::vector<unsigned int>& rhs)
    {
        return euphoria::tests::VectorEquals
        (
            lhs,
            rhs,
            [](unsigned int f) -> std::string { return Str() << f; },
            [](unsigned int a, unsigned int b) -> FalseString
            {
                if(a == b)
                {
                    return FalseString::True();
                }
                else
                {
                    return FalseString::False
                    (
                        Str() << a << " != " << b
                    );
                }
            }
        );
    }


    FalseString
    VectorEquals(const std::vector<float> lhs, const std::vector<float>& rhs)
    {
        return euphoria::tests::VectorEquals
        (
            lhs,
            rhs,
            [](float f) -> std::string { return Str() << f; },
            [](float a, float b) -> FalseString
            {
                if(euphoria::tests::approx(a) == b)
                {
                    return FalseString::True();
                }
                else
                {
                    return FalseString::False
                    (
                        Str() << a << " != " << b
                    );
                }
            }
        );
    }
}


TEST_CASE("bufferbuilder2d-default-is-empty", "[bufferbuilder2d]")
{
    auto bb = BufferBuilder2d{};
    CHECK(bb.data.empty());
    CHECK(bb.tris.empty());
}

TEST_CASE("bufferbuilder2d-add-points", "[bufferbuilder2d]")
{
    auto bb = BufferBuilder2d{};
    bb.AddVertex({0.0f, 1.0f, 2.0f, 3.0f});
    bb.AddVertex({55.0f, 20.0f, 0.0f, -2.0f});
    CHECK(VectorEquals(bb.data, {0.0f, 1.0f, 2.0f, 3.0f, 55.0f, 20.0f, 0.0f, -2.0f}));
    CHECK(bb.tris.empty());
}

TEST_CASE("bufferbuilder2d-triangles", "[bufferbuilder2d]")
{
    auto bb = BufferBuilder2d{};
    bb.AddVertex({0.0f, 0.0f, 0.0f, 0.0f});
    bb.AddVertex({0.0f, 1.0f, 0.0f, 0.0f});
    bb.AddVertex({1.0f, 0.0f, 0.0f, 0.0f});
    CHECK(bb.data.size() == 12);
    SECTION("ok - ccw")
    {
        bb.AddTriangle(0, 1, 2);
        CHECK(VectorEquals(bb.tris, {0, 1, 2}));
    }
    SECTION("bad - cw")
    {
        CHECK_THROWS(bb.AddTriangle(0, 2, 1));
    }
}

TEST_CASE("bufferbuilder2d-quad", "[bufferbuilder2d]")
{
    auto bb = BufferBuilder2d{};
    const auto z = Point{0.0f, 0.0f, 0.0f, 0.0f};
    const auto x = Point{1.0f, 0.0f, 0.0f, 0.0f};
    const auto y = Point{0.0f, 1.0f, 0.0f, 0.0f};
    const auto xy = Point{1.0f, 1.0f, 0.0f, 0.0f};
    SECTION("bad - cw")
    {
        CHECK_THROWS(bb.AddQuad(z, y, xy, x));
    }
    SECTION("bad - ccw")
    {
        CHECK_THROWS(bb.AddQuad(z, x, xy, y));
    }
    SECTION("ok bottom-top, left-right")
    {
        bb.AddQuad(z, x, y, xy);
        // todo(Gustav): add checks
    }
    SECTION("bad - top->bottom, left->right")
    {
        CHECK_THROWS(bb.AddQuad(y, xy, z, x));
    }
}

