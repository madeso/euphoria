#include "eu/base/transform.h"

#include "eu/tests/approx_equal.h"

#include "catch2/catch_all.hpp"

using namespace eu::tests;
using namespace eu;

// todo(Gustav): too many tests? remove duplicates?

TEST_CASE("transform-identity", "[transform]")
{
    constexpr auto src = Transform
    {
        .position = {0, 0, 0},
        .rotation = q_identity,
        .scale = {1, 1, 1}
    };

    const auto mat = matrix_from_transform(src);
    CHECK(mat == approx(m4_identity));

    const auto ret = transform_from_matrix(mat);
    CHECK(ret.position == approx(src.position));
    CHECK(ret.rotation == approx(src.rotation));
    CHECK(ret.scale == approx(src.scale));
}

TEST_CASE("transform-translation-only", "[transform]")
{
    constexpr auto src = Transform
    {
        .position = {1, 2, 3},
        .rotation = q_identity,
        .scale = {1, 1, 1}
    };

    const auto mat = matrix_from_transform(src);
    CHECK(mat.get_transformed_point(v3{0, 0, 0}) == approx(v3{1, 2, 3}));

    const auto ret = transform_from_matrix(mat);
    CHECK(ret.position == approx(src.position));
    CHECK(ret.rotation == approx(src.rotation));
    CHECK(ret.scale == approx(src.scale));
}

TEST_CASE("transform-scale-only", "[transform]")
{
    constexpr auto src = Transform
    {
        .position = {0, 0, 0},
        .rotation = q_identity,
        .scale = {2, 3, 4}
    };

    const auto mat = matrix_from_transform(src);
    CHECK(mat.get_transformed_point(v3{1, 1, 1}) == approx(v3{2, 3, 4}));

    const auto ret = transform_from_matrix(mat);
    CHECK(ret.position == approx(src.position));
    CHECK(ret.rotation == approx(src.rotation));
    CHECK(ret.scale == approx(src.scale));
}

TEST_CASE("transform-rotation-matches-quaternion-rotation", "[transform]")
{
    const auto rotation = Q::from(rha(kk::up, 90_deg))
        .then_get_rotated(Q::from(rha(kk::right, 45_deg)));
    
    const auto trans = Transform
    {
        .position = {0, 0, 0},
        .rotation = rotation,
        .scale = {1, 1, 1}
    };
    const auto mat = matrix_from_transform(trans);

    CHECK(mat.get_x_axis() == approx(rotation.get_local_right()));
    CHECK(mat.get_y_axis() == approx(rotation.get_local_up()));
    CHECK(mat.get_z_axis() == approx(rotation.get_local_out()));
}

TEST_CASE("transform-roundtrip", "[transform]")
{
    const auto rotation = GENERATE
    (
        q_identity,
        Q::from(rha(kk::up, 90_deg)),
        Q::from(rha(kk::right, -45_deg)),
        Q::from(rha(kk::in, 30_deg)),
        Q::from(rha(kk::up, 45_deg)).then_get_rotated(Q::from(rha(kk::right, 60_deg))),
        Q::from(rha(kk::right, 20_deg)).then_get_rotated(Q::from(rha(kk::up, -70_deg))).then_get_rotated(Q::from(rha(kk::in, 15_deg)))
    );

    const auto position = GENERATE
    (
        v3{0, 0, 0},
        v3{1, 2, 3},
        v3{-5, 4, -2}
    );

    const auto scale = GENERATE
    (
        v3{1, 1, 1},
        v3{2, 3, 4},
        v3{0.5f, 1.5f, 2.5f}
    );

    const auto trans = Transform
    {
        .position = position,
        .rotation = rotation,
        .scale = scale
    };
    const auto mat = matrix_from_transform(trans);
    const auto ret = transform_from_matrix(mat);

    CHECK(ret.position == approx(trans.position));
    CHECK(ret.rotation == approx(trans.rotation));
    CHECK(ret.scale == approx(trans.scale));

    CHECK(matrix_from_transform(ret) == approx(mat));
}

TEST_CASE("transform-from-matrix-of-known-rotation", "[transform]")
{
    const auto mat = m4::from(rha(kk::up, 90_deg));
    const auto trans = transform_from_matrix(mat);

    CHECK(trans.position == approx(v3{0, 0, 0}));
    CHECK(trans.scale == approx(v3{1, 1, 1}));
    CHECK(trans.rotation == approx(Q::from(rha(kk::up, 90_deg))));
}

TEST_CASE("transform-combined", "[transform]")
{
    const auto rotation = Q::from(rha(kk::up, 90_deg));
    const auto trans = Transform
    {
        .position = {5, 0, 0},
        .rotation = rotation,
        .scale = {2, 2, 2}
    };
    const auto mat = matrix_from_transform(trans);

    CHECK(mat.get_transformed_point(v3{1, 0, 0}) == approx(v3{5, 0, -2}));
}
