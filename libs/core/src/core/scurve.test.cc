#include "core/scurve.h"

#include "catch2/catch_test_macros.hpp"

#include "tests/equaler.h"

using namespace eu::core;
using namespace eu::tests;


FalseString is_equal(const SCurve& lhs, SCurve rhs)
{
	return Equaler{lhs, rhs}
		.add(
			"s", [](const SCurve& s) { return s.slope; }, machine_epsilon
		)
		.add(
			"t", [](const SCurve& s) { return s.threshold; }, machine_epsilon
		)
		.resolve();
}

TEST_CASE("scurve_from_input", "[scurve]")
{
	const auto middle = s_curve_from_input(0.5f, 0.5f);
    CHECK(is_equal(middle, {1.0f, 0.75f}));

    const auto top_right = s_curve_from_input(1.0f, 1.0f);
	CHECK(is_equal(top_right, {243.0f, 0.5f}));

    const auto bottom_right = s_curve_from_input(1.0f, 0.0f);
	CHECK(is_equal(bottom_right, {243.0f, 1.0f}));
}
