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
    CHECK(bb.data.size() == 0);
    CHECK(bb.tris.size() == 0);
}

TEST_CASE("bufferbuilder2d-add-points", "[bufferbuilder2d]")
{
    auto bb = BufferBuilder2d{};
    bb.AddVertex({0.0f, 1.0f, 2.0f, 3.0f});
    bb.AddVertex({55.0f, 20.0f, 0.0f, -2.0f});
    CHECK(VectorEquals(bb.data, {0.0f, 1.0f, 2.0f, 3.0f, 55.0f, 20.0f, 0.0f, -2.0f}));
    CHECK(bb.tris.size() == 0);
}

TEST_CASE("bufferbuilder2d-triangles", "[bufferbuilder2d]")
{
    auto bb = BufferBuilder2d{};
    bb.AddTriangle(11, 21, 13);
    bb.AddTriangle(42, 66, 99);
    CHECK(VectorEquals(bb.tris, {11, 21, 13, 42, 66, 99}));
    CHECK(bb.data.size() == 0);
}

TEST_CASE("bufferbuilder2d-quad", "[bufferbuilder2d]")
{
    auto bb = BufferBuilder2d{};
    // todo(Gustav): add proper quad here
    bb.AddQuad
    (
        Point{0.0f, 0.0f, 0.0f, 0.0f},
        Point{0.0f, 0.0f, 0.0f, 0.0f},
        Point{0.0f, 0.0f, 0.0f, 0.0f},
        Point{0.0f, 0.0f, 0.0f, 0.0f}
    );
}

